/*********************************************************************************
 PlotFitBase.cpp

	PlotFitBase class.

	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#include "PlotFitBase.h"

#include <jx-af/j2dplot/J2DPlotData.h>
#include <jx-af/j2dplot/J2DPlotWidget.h>
#include <jx-af/j2dplot/J2DPlotDataBase.h>

#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/JMatrix.h>
#include <jx-af/jcore/JVector.h>

#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jAssert.h>

const JSize	ITMAX	= 100;
const JFloat	CGOLD	= 0.3819660;
const JFloat	ZEPS	= 1.0e-12;
const JFloat	TOLL	= 1.0e-10;
const JFloat	GOLD	= std::numbers::phi;
const JFloat	IMIT	= 100.0;
const JFloat	TINY	= 1.0e-20;
const JFloat	BIG		= 1.0e30;
const JFloat	SAFE	= 2.0;
const JFloat	CON		= 1.4;


/*********************************************************************************
 Constructor

 ********************************************************************************/

PlotFitBase::PlotFitBase
	(
	J2DPlotWidget*		plot,
	J2DPlotDataBase*	fitData,
	const JFloat		xMin,
	const JFloat		xMax
	)
	:
	PlotFitFunction(plot, fitData, xMin, xMax)
{
	itsUsingRange = false;
	JPlotFitBaseX(fitData);
}

PlotFitBase::PlotFitBase
	(
	J2DPlotWidget* plot,
	J2DPlotDataBase* fitData,
	const JFloat xmin,
	const JFloat xmax,
	const JFloat ymin,
	const JFloat ymax
	)
	:
	PlotFitFunction(plot, fitData, xmin, xmax)
{
	if (xmax > xmin)
	{
		itsRangeXMax = xmax;
		itsRangeXMin = xmin;
	}
	else
	{
		itsRangeXMax = xmin;
		itsRangeXMin = xmax;
	}
	if (ymax > ymin)
	{
		itsRangeYMax = ymax;
		itsRangeYMin = ymin;
	}
	else
	{
		itsRangeYMax = ymin;
		itsRangeYMin = ymax;
	}
	itsUsingRange = true;
	JPlotFitBaseX(fitData);
}

void
PlotFitBase::JPlotFitBaseX
	(
	J2DPlotDataBase* fitData
	)
{
	itsCurrentConstantParmIndex	= 0;
	itsUseAltFunction			= false;

	if (fitData->HasXErrors() || fitData->HasYErrors())
	{
		SetHasParameterErrors(true);
	}
	else
	{
		SetHasParameterErrors(false);
	}

	itsRealData = jnew JArray<J2DDataPoint>;
	const JSize count = fitData->GetItemCount();
	for (JSize i=1; i<= count; i++)
	{
		J2DDataPoint point;
		if (GetDataElement(i, &point))
		{
			itsRealData->AppendItem(point);
		}
	}
//	GenerateDiffData();
//	AdjustDiffData();
}


/*********************************************************************************
 Destructor

 ********************************************************************************/

PlotFitBase::~PlotFitBase()
{
}

/*********************************************************************************
 GetGoodnessOfFitName

 ********************************************************************************/

bool
PlotFitBase::GetGoodnessOfFitName
	(
	JString* name
	)
	const
{
	const J2DPlotDataBase* data = GetData();
	if (data->HasXErrors() || data->HasYErrors())
	{
		*name = "Chi^2/(N-3)";
	}
	else
	{
		*name = "Std dev";
	}
	return true;
}

/*********************************************************************************
 GetGoodnessOfFit

 ********************************************************************************/

bool
PlotFitBase::GetGoodnessOfFit
	(
	JFloat* value
	)
	const
{
	const J2DPlotDataBase* data = GetData();
	if (data->HasXErrors() || data->HasYErrors())
	{
		*value = itsChi2/(itsRealData->GetItemCount() - GetParameterCount());
	}
	else
	{
		*value = GetStdDev();
	}

	return true;
}

/*********************************************************************************
 GenerateFit

 ********************************************************************************/

void
PlotFitBase::GenerateFit
	(
	const JVector&	parameters,
	const JFloat	chi2
	)
{
	JVector p(parameters);
	JVector err(p.GetDimensionCount());

	itsChi2	= chi2;
	const JSize n = p.GetDimensionCount();
	JMatrix xi(n,n);
	JSize iter;

	for (JIndex i = 1; i <= 3; i++)
	{
		iter = 0;
		for (JIndex j = 1; j <= n; j++)
		{
			xi.SetElement(j,j,1.0);
		}
		MinimizeN(&p, &xi, &iter);
	}

//	std::cout << "Parms: " << p << std::endl;

	itsChi2 = ChiSqr(p);

	for (JIndex i = 1; i <= n; i++)
	{
		err.SetElement(i, CalcError(p, i));
	}

	SetCurrentParameters(p);
	SetErrors(err);
	GenerateDiffData();
}

/*********************************************************************************
 CalcError (private)

 ********************************************************************************/

JFloat
PlotFitBase::CalcError
	(
	const JVector&	parameters,
	const JIndex	constIndex
	)
{
	itsCurrentConstantParmIndex	= constIndex;
	itsCurrentConstantParm		= parameters.GetElement(constIndex);
	JFloat currentParm			= itsCurrentConstantParm;

	const JSize n = parameters.GetDimensionCount() - 1;
	JVector p(n);
	JMatrix xi(n,n);
	for (JIndex i = 1; i <= n; i++)
	{
		xi.SetElement(i,i,1.0);
	}
	for (JIndex i = 1; i <= itsCurrentConstantParmIndex - 1; i++)
	{
		p.SetElement(i, parameters.GetElement(i));
	}
	for (JIndex i = itsCurrentConstantParmIndex + 1; i <= parameters.GetDimensionCount(); i++)
	{
		p.SetElement(i - 1, parameters.GetElement(i));
	}
	JVector pSav(p);
	JMatrix xiSav(xi);

	// here we need to generate an first guess for the error
	itsUseAltFunction	= true;
	itsChiPlus			= itsChi2 + 1;
	const JSize nS		= parameters.GetDimensionCount();
	JFloat sig;
	JVector pS(parameters);
	JMatrix xiS(nS, nS);
	for (JIndex i = 1; i <= nS; i++)
	{
		xiS.SetElement(i,i,1.0);
	}
	JFloat a, b, c, f1, f2, f3;
	a	= currentParm;
	b	= currentParm * 1.01;
//	std::cout << "Calculating error for parameter: " << constIndex << std::endl;
	Bracket(&a, &b, &c, &f1, &f2, &f3, pS, xiS.GetColVector(itsCurrentConstantParmIndex));
	Minimize(a, b, c, pS, xiS.GetColVector(itsCurrentConstantParmIndex), &sig);
	sig =  fabs(sig)/10 * JSign(currentParm);
//	std::cout << "Calculating error for parameter: " << constIndex << std::endl;
//	std::cout << "Starting guess: " << sig << std::endl;
	itsUseAltFunction	= false;

	itsCurrentConstantParm	= currentParm + sig;

	JFloat chiplus	= sqrt(itsChiPlus);
	JIndex i = 0;
	bool ok = true;
	JSize iter;
	JFloat chitemp = MinimizeN(&p, &xi, &iter);
//	std::cout << "Chitemp start: " << chitemp << std::endl;
	JFloat lastchi = 0;

	do
	{
		if (chitemp > chiplus)
		{
			ok	= false;
			p	= pSav;
			xi	= xiSav;
		}
		else
		{
			lastchi = chitemp;
			sig *= 10;
			itsCurrentConstantParm = currentParm + sig;
			iter	= 0;
//			pSav	= p;
//			xiSav	= xi;
			p	= pSav;
			xi	= xiSav;
			chitemp = MinimizeN(&p, &xi, &iter);
//			std::cout << "Chitemp 10s: " << chitemp << std::endl;
//			std::cout << "Current sig: " << sig << std::endl;
			i++;
		}
	}
	while (i < 20 && ok);

	sig /= 10;

	i = 2;
	JFloat chi1, chi2, chi3 = lastchi;
	JFloat tsig;
	ok = true;
	do
	{
		chi1 = chi3;
		itsCurrentConstantParm = currentParm + sig * i;
		iter	= 0;
//		pSav	= p;
//		xiSav	= xi;
		p	= pSav;
		xi	= xiSav;
		chitemp = MinimizeN(&p, &xi, &iter);
//		std::cout << "Chitemp 1s: " << chitemp << std::endl;
		chi3 = chitemp;
		if (chitemp > chiplus)
		{
			JFloat x1 = sig*(i - 1);
			JFloat x2 = sig*(i - 0.5);
			JFloat x3 = sig*i;
			p	= pSav;
			xi	= xiSav;
			itsCurrentConstantParm = currentParm + x2;
			chi2 = MinimizeN(&p, &xi, &iter);
//			std::cout << "***" << chiplus << "-:- " << x1 << ": " << chi1 << ' ';
//			std::cout << x2 << ": " << chi2 << ' ';
//			std::cout << x3 << ": " << chi3 << std::endl;
			iter	= 0;
			JFloat e1 = (chi3*x1*(x1-x2)*x2+x3*(chi1*x2*(x2-x3)+chi2*x1*(-x1+x3)))/
						((x1-x2)*(x1-x3)*(x2-x3));
			JFloat e2 = (chi3*(-x1*x1+x2*x2)+chi2*(x1*x1-x3*x3)+chi1*(-x2*x2+x3*x3))/
						((x1-x2)*(x1-x3)*(x2-x3));
			JFloat e3 = (chi3*(x1-x2)+chi1*(x2-x3)+chi2*(-x1+x3))/
						((x1-x2)*(x1-x3)*(x2-x3));
			tsig = fabs((-e2+JSign(currentParm)*sqrt(e2*e2+4*e3*(chiplus-e1)))/2/e3);
			return tsig;
			ok = false;
		}
		i++;
	}
	while (i <= 10 && ok);

	return 0;
/*

	ok = true;
	JFloat temp = sig;
	i = 2;
	do
	{
		if (chitemp > chiplus)
		{
			ok = false;
			p	= pSav;
			xi	= xiSav;
		}
		else
		{
			lastchi = chitemp;
			itsCurrentConstantParm = currentParm + sig * i;
			iter	= 0;
			pSav	= p;
			xiSav	= xi;
			chitemp = MinimizeN(&p, &xi, &iter);
			i++;
		}
	}
	while (i <= 10 && ok);
	sig *= i - 2;

	JFloat j = 1.1;
	JFloat chi1, chi2, chi3 = lastchi;
	JFloat tsig;
	ok = true;
	do
	{
		chi1 = chi3;
		itsCurrentConstantParm = currentParm + sig * j;
		iter	= 0;
		chitemp = MinimizeN(&p, &xi, &iter);
		chi3 = chitemp;
		if (chitemp > chiplus)
		{
			JFloat x1 = sig*(j-0.1);
			JFloat x2 = sig*(j-0.05);
			JFloat x3 = sig*(j);
			std::cout << "***" << x1 << ' ' << x2 << ' ' << x3 << std::endl;
			itsCurrentConstantParm = currentParm + x2;
			chi2 = MinimizeN(&p, &xi, &iter);
			iter	= 0;
			JFloat e1 = (chi3*x1*(x1-x2)*x2+x3*(chi1*x2*(x2-x3)+chi2*x1*(-x1+x3)))/
						((x1-x2)*(x1-x3)*(x2-x3));
			JFloat e2 = (chi3*(-x1*x1+x2*x2)+chi2*(x1*x1-x3*x3)+chi1*(-x2*x2+x3*x3))/
						((x1-x2)*(x1-x3)*(x2-x3));
			JFloat e3 = (chi3*(x1-x2)+chi1*(x2-x3)+chi2*(-x1+x3))/
						((x1-x2)*(x1-x3)*(x2-x3));
			tsig = fabs((-e2+JSign(currentParm)*sqrt(e2*e2+4*e3*(chiplus-e1)))/2/e3);
			return tsig;
			ok = false;
		}
		j += 0.1;
	}
	while ((j < 2) && ok);

	return 0;*/
}

/*********************************************************************************
 ChiSqr

 ********************************************************************************/

JFloat
PlotFitBase::ChiSqr
	(
	const JVector& p
	)
{
	if (!itsUseAltFunction && itsCurrentConstantParmIndex != 0)
	{
		JVector pAlt(p.GetDimensionCount() + 1);
		for (JIndex i = 1; i <= itsCurrentConstantParmIndex - 1; i++)
		{
			pAlt.SetElement(i, p.GetElement(i));
		}
		pAlt.SetElement(itsCurrentConstantParmIndex, itsCurrentConstantParm);
		for (JIndex i = itsCurrentConstantParmIndex + 1; i <= pAlt.GetDimensionCount(); i++)
		{
			pAlt.SetElement(i, p.GetElement(i - 1));
		}
		SetCurrentParameters(pAlt);
	}
	else
	{
		SetCurrentParameters(p);
	}

	J2DDataPoint point;
	JSize rcount = itsRealData->GetItemCount();
	JFloat c = 0;

	for (JSize i = 1; i <= rcount; i++)
	{
		point = itsRealData->GetItem(i);
		JFloat sy = point.yerr;
		JFloat sx = point.xerr;
		if (sy == 0 && sx == 0)
		{
			sy = 1;
		}
//		JFloat e = pow(point.xerr * (FunctionNPrimed(point.x)),2) + sy * sy;
//		c += pow(point.y - FunctionN(point.x), 2)/e;
		JFloat e = point.xerr * FunctionNPrimed(point.x);
		e *= e;
		e += sy * sy;
		JFloat tc = point.y - FunctionN(point.x);
		tc *= tc;
		c += tc/e;
	}
	return c;
}

/*********************************************************************************
 Function

 ********************************************************************************/

JFloat
PlotFitBase::Function
	(
	JFloat				Bt,
	const JVector&		p,
	const JVector&		xi
	)
{
	JVector xt(p);
	xt = p + Bt * xi;
	JFloat value = ChiSqrSqrt(xt);
	return value;
}

/*********************************************************************************
 ChiSqrSqrt

 ********************************************************************************/

JFloat
PlotFitBase::ChiSqrSqrt
	(
	const JVector& parameters
	)
{
	if (itsUseAltFunction)
	{
		return sqrt(fabs(ChiSqr(parameters) - itsChiPlus));
	}
	return sqrt(ChiSqr(parameters));
}


/*********************************************************************************
 Minimize

 ********************************************************************************/

JFloat
PlotFitBase::Minimize
	(
	JFloat			ax,
	JFloat			bx,
	JFloat			cx,
	const JVector&	parms,
	const JVector&	xi,
	JFloat*			xmin
	)
{
	JFloat oldstep=0, fv=0, step=0;

	JFloat x = bx, w = bx, v = bx;

	JFloat fx = Function(bx, parms, xi);
	JFloat fw = fx;
	JFloat low = JMin(ax,cx);
	JFloat high = JMax(ax,cx);

	JSize iter=1;
	while (iter <= ITMAX)
	{
		const JFloat middle=0.5*(low+high);
		const JFloat tol1=TOLL*fabs(x)+ZEPS;
		const JFloat tol2 = 2.0*tol1;
		if (fabs(x-middle) <= tol2-0.5*(high-low))
		{
			*xmin= x;
			return fx;
		}
		if (fabs(oldstep) > tol1)
		{
			const JFloat r=(x - w) * (fx - fv);
			JFloat q=(x - v) * (fx - fw);
			JFloat p=(x - v) * q + (w - x)* r;
			q= 2.0 * (q - r);
			if (q > 0.0)
			{
				p = -p;
			}
			q = fabs(q);
			const JFloat steptemp = oldstep;
			oldstep = step;
			if (fabs(p) >= fabs(0.5*q*steptemp) || p <= q*(low-x) || p >= q*(high-x))
			{
				if (x >= middle)
				{
					oldstep = low - x;
				}
				else
				{
					oldstep = high - x;
				}
				step = CGOLD*oldstep;
			}
			else
			{
				step = p/q;
				const JFloat u = x + step;
				if (u-low < tol2 || high-u < tol2)
				{
					if (middle - x > 0.0)
					{
						step = fabs(tol1);
					}
					else
					{
						step = -fabs(tol1);
					}
				}
			}
		}
		else
		{
			if (x >= middle)
			{
				oldstep = low - x;
			}
			else
			{
				oldstep = high - x;
			}
			step = CGOLD*oldstep;
		}

		const JFloat u =
			fabs(step) >= tol1 ? x + step :
			step > 0.0 ? x + fabs(tol1) : x - fabs(tol1);

		const JFloat fu = Function(u, parms, xi);
		if (fu <= fx)
		{
			if (u >= x)
			{
				low=x;
			}
			else
			{
				high=x;
			}
			v = w;
			w = x;
			x = u;
			fv = fw;
			fw = fx;
			fx = fu;
		}
		else
		{
			if (u < x)
			{
				low = u;
			}
			else
			{
				high = u;
			}

			if (fu <= fw || w == x)
			{
				v = w;
				w = u;
				fv = fw;
				fw = fu;
			}
			else if (fu <= fv || v == x || v == w)
			{
				v = u;
				fv = fu;
			}
		}

		iter++;
	}

	*xmin = x;
	return fx;
}

/*********************************************************************************
 MinimizeN

 ********************************************************************************/

JFloat
PlotFitBase::MinimizeN
	(
	JVector*	p,
	JMatrix*	xi,
	JSize*		iter
	)
{
	const JIndex n = p->GetDimensionCount();

	JVector pt(*p);
	JVector ptt(n);
	JVector xit(n);

	JFloat fret = ChiSqrSqrt(*p);

	for (*iter = 1; *iter <= ITMAX; ++(*iter))
	{
		JFloat fp = fret;
		JIndex ibig=0;
		JFloat del=0.0;
		JFloat fptt;
		for (JIndex i = 1; i <= n; i++)
		{
			xit = xi->GetColVector(i);
			fptt = fret;
			fret = LinearMinimization(p, &xit);
			if (fabs(fptt-fret) > del)
			{
				del=fabs(fptt-fret);
				ibig=i;
			}
		}
		if (2.0*fabs(fp-fret) <= TOLL*(fabs(fp)+fabs(fret)))
		{
			return fret;
		}
		ptt = 2.0 * *p - pt;
		xit = *p - pt;
		pt = *p;
		fptt = ChiSqrSqrt(ptt);
		if (fptt < fp)
		{
			const JFloat t = 2.0*(fp - 2.0*fret+fptt) * (fp-fret-del)*(fp-fret-del) -del*(fp-fptt)*(fp-fptt);
			if (t < 0.0)
			{
				fret = LinearMinimization(p, &xit);
				xi->SetColVector(ibig, xi->GetColVector(n));
				xi->SetColVector(n, xit);
			}
		}
	}
	return fret;
}

/*********************************************************************************
 LinearMinimization

 ********************************************************************************/

JFloat
PlotFitBase::LinearMinimization
	(
	JVector* p,
	JVector* xi
	)
{
	JFloat xx,xmin,fx,fb,fa,bx,ax, fret;

	ax=0.0;
	//xx=1.0;
	xx=.1;
//	std::cout << "Starting... " << std::endl;
	Bracket(&ax,&xx,&bx,&fa,&fx,&fb, *p, *xi);
//	std::cout << "ax: " << ax << std::endl;
//	std::cout << "xx: " << xx << std::endl;
//	std::cout << "bx: " << bx << std::endl;
	fret = Minimize(ax,xx,bx,*p, *xi,&xmin);
//	std::cout << "Xmin: " << xmin << std::endl;
	*xi *= xmin;
//	std::cout << "Xi: " << *xi << std::endl;
	*p += *xi;
//	std::cout << "P: " << *p << std::endl;

	return fret;
}

/*********************************************************************************
 Bracket

 ********************************************************************************/

void
PlotFitBase::Bracket
	(
	JFloat			*ax,
	JFloat			*bx,
	JFloat			*cx,
	JFloat			*fa,
	JFloat			*fb,
	JFloat			*fc,
	const JVector&		p,
	const JVector&		xi
	)
{
	JFloat ulim,u,r,q,fu,dum;

	*fa = Function(*ax, p, xi);
	*fb = Function(*bx, p, xi);

	if (*fb > *fa)
	{
		Shift(dum,*ax,*bx,dum);
		Shift(dum,*fb,*fa,dum);
	}
	*cx = (*bx)+GOLD*(*bx-*ax);
	*fc = Function(*cx, p, xi);

	while (*fb > *fc)
	{
		r = (*bx-*ax)*(*fb-*fc);
		q = (*bx-*cx)*(*fb-*fa);
		int sign;
		if (q-r > 0.0)
		{
			sign = 1;
		}
		else
		{
			sign = -1;
		}
		u = (*bx)-((*bx-*cx)*q-(*bx-*ax)*r)/
			(2.0*sign*JMax(fabs(q-r),TINY));
		ulim = (*bx)+IMIT*(*cx-*bx);
		if ((*bx-u)*(u-*cx) > 0.0)
		{
			fu = Function(u, p, xi);
			if (fu < *fc)
			{
				*ax = (*bx);
				*bx = u;
				*fa = (*fb);
				*fb = fu;
				return;
			}
			else if (fu > *fb)
			{
				*cx = u;
				*fc = fu;
				return;
			}
			u = (*cx)+GOLD*(*cx-*bx);
			fu = Function(u, p, xi);
		}
		else if ((*cx-u)*(u-ulim) > 0.0)
		{
			fu = Function(u, p, xi);
			if (fu < *fc)
			{
				JFloat temp = *cx+GOLD*(*cx-*bx);
				Shift(*bx,*cx,u,temp);
				temp = Function(u, p, xi);
				Shift(*fb,*fc,fu,temp);
			}
		}
		else if ((u-ulim)*(ulim-*cx) >= 0.0)
		{
			u = ulim;
			fu = Function(u, p, xi);
		}
		else
		{
			u = (*cx)+GOLD*(*cx-*bx);
			fu = Function(u, p, xi);
		}
		Shift(*ax,*bx,*cx,u);
		Shift(*fa,*fb,*fc,fu);
	}
}

void
PlotFitBase::Shift
	(
	JFloat& a,
	JFloat& b,
	JFloat& c,
	JFloat& d
	)
{
	a = b;
	b = c;
	c = d;
}

/*********************************************************************************
 DataElementValid

 ********************************************************************************/

bool
PlotFitBase::DataElementValid
	(
	const JIndex index
	)
{
	const J2DPlotDataBase* data = GetData();
	J2DDataPoint point;
	data->GetItem(index, &point);

	if (itsUsingRange)
	{
		if ((point.x >= itsRangeXMin) &&
			(point.x <= itsRangeXMax) &&
			(point.y >= itsRangeYMin) &&
			(point.y <= itsRangeYMax))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return true;
}

/******************************************************************************
 GetDataElement

 *****************************************************************************/

bool
PlotFitBase::GetDataElement
	(
	const JIndex index,
	J2DDataPoint* point
	)
{
	bool valid = DataElementValid(index);
	if (!valid)
	{
		return false;
	}
	const J2DPlotDataBase* data = GetData();
	data->GetItem(index, point);
	return true;
}

/*****************************************************************************
 AdjustDiffData

 *****************************************************************************/

void
PlotFitBase::AdjustDiffData()
{
}

/******************************************************************************
 GetRealElement (protected)

 ******************************************************************************/

J2DDataPoint
PlotFitBase::GetRealElement
	(
	const JIndex index
	)
{
	return itsRealData->GetItem(index);
}

/******************************************************************************
 GetRealElementCount (protected)

 ******************************************************************************/

JSize
PlotFitBase::GetRealElementCount()
{
	return itsRealData->GetItemCount();
}

/******************************************************************************
 FunctionNPrimed (virtual protected)

 ******************************************************************************/

JFloat
PlotFitBase::FunctionNPrimed
	(
	const JFloat x
	)
{
	JFloat y = 0;
	const JIndex matrixSize	= 10;
	JMatrix	a(matrixSize, matrixSize);
	JFloat xmin, xmax;
	GetXRange(&xmin, &xmax);
	JFloat delta = (xmax - xmin)/100;
	a.SetElement(1, 1, (FunctionN(x + delta) - FunctionN(x - delta))/(2.0 * delta));
	JFloat err	= BIG;
	for (JIndex i = 2; i <= matrixSize; i++)
	{
		delta /= CON;
		a.SetElement(1, i, (FunctionN(x + delta) - FunctionN(x - delta))/(2.0 * delta));
		JFloat fac	= CON * CON;
		for (JIndex j = 2; j <= i; j++)
		{
			a.SetElement(j, i, (a.GetElement(j - 1, i) * fac - a.GetElement(j - 1, i - 1))/(fac - 1.0));
			fac	= CON * CON * fac;
			JFloat errt	=
				JMax(fabs(a.GetElement(j, i) - a.GetElement(j - 1, i)),
					 fabs(a.GetElement(j, i) - a.GetElement(j - 1, i - 1)));
			if (errt <= err)
			{
				err	= errt;
				y	= a.GetElement(j, i);
			}
		}
		if (fabs(a.GetElement(i, i) - a.GetElement(i - 1, i - 1)) >= SAFE * err)
		{
			return y;
		}
	}
	return y;
}

/******************************************************************************
 GetFunctionString (virtual public)

 ******************************************************************************/

JString
PlotFitBase::GetFunctionString()
	const
{
	return itsFunctionStr;
}

/******************************************************************************
 GetFitFunctionString (virtual public)

 ******************************************************************************/

JString
PlotFitBase::GetFitFunctionString()
	const
{
	return itsFunctionStr;
}

/******************************************************************************
 SetFunctionString (protected)

 ******************************************************************************/

void
PlotFitBase::SetFunctionString
	(
	const JString& str
	)
{
	itsFunctionStr	= str;
}

/*********************************************************************************
 GenerateDiffData (protected)

 ********************************************************************************/

void
PlotFitBase::GenerateDiffData()
{
	JArray<JFloat> xdata;
	JArray<JFloat> ydata;
	JArray<JFloat> xerrdata;
	JArray<JFloat> yerrdata;
	if (!GetData()->HasYErrors())
	{
		CalculateStdDev();
	}
	const JSize count = GetData()->GetItemCount();
	J2DDataPoint data;
	for (JSize i = 1; i <= count; i++)
	{
		if (GetDataElement(i, &data))
		{
			JFloat fitY;
			GetYValue(data.x, &fitY);
			xdata.AppendItem(data.x);
			ydata.AppendItem(data.y - fitY);
			if (GetData()->HasYErrors())
			{
				if (GetData()->HasXErrors())
				{
					JFloat b	= FunctionNPrimed(data.x);
					JFloat err	= sqrt(data.yerr * data.yerr + b * b * data.xerr * data.xerr);
					yerrdata.AppendItem(err);
				}
				else
				{
					yerrdata.AppendItem(data.yerr);
				}
			}
			else
			{
				yerrdata.AppendItem(GetStdDev());
			}
		}
	}
	J2DPlotData* pdata;
	J2DPlotData::Create(&pdata, xdata, ydata, false);
	pdata->SetYErrors(yerrdata);
	if (GetData()->HasXErrors())
	{
		pdata->SetXErrors(xerrdata);
	}
	SetDiffData(pdata);
}
