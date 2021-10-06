/*********************************************************************************
 PlotFitBase.h
 
	Interface for the PlotFitBase class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_PlotFitBase
#define _H_PlotFitBase

#include <PlotFitFunction.h>
#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/JString.h>

class J2DPlotWidget;
class JVector;
class JMatrix;

class PlotFitBase : public PlotFitFunction
{
public:

	PlotFitBase(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	PlotFitBase(J2DPlotWidget* plot, J2DPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~PlotFitBase();	

	bool	GetGoodnessOfFitName(JString* name) const override;
	bool	GetGoodnessOfFit(JFloat* value) const override;
	
	void				AdjustDataRange(const JFloat xmin, const JFloat xmax,
										const JFloat ymin, const JFloat ymax);

	JString		GetFunctionString() const override;
	JString		GetFitFunctionString() const override;

	void				GenerateFit(const JVector& parameters, const JFloat chi2);

protected:

	JFloat				Minimize(	JFloat ax, 
									JFloat bx, 
									JFloat cx,
									const JVector& p, 
									const JVector& xi,
									JFloat* xmin);
	JFloat				MinimizeN(	JVector* p, 
									JMatrix* xi, 
									JSize *iter);
	JFloat				LinearMinimization(	JVector* p, 
											JVector* xi);
	void				Bracket (	JFloat *ax, 
									JFloat *bx, 
									JFloat *cx, 
									JFloat *fa, 
									JFloat *fb, 
									JFloat *fc,
									const JVector& p, 
									const JVector& xi);
	void				Shift(	JFloat& a,
								JFloat& b,
								JFloat& c,
								JFloat& d);
	virtual JFloat		ChiSqrSqrt(const JVector& parameters);
	virtual JFloat		Function(JFloat Bt,
								 const JVector& p, 
								 const JVector& xi);
	virtual JFloat		ChiSqr(const JVector& p);

	void 		GenerateDiffData() override;

	bool	DataElementValid(const JIndex index) override;
	bool	GetDataElement(const JIndex index, J2DDataPoint* point) override;

	J2DDataPoint		GetRealElement(const JIndex index);
	JSize				GetRealElementCount();

	virtual void		SetCurrentParameters(const JVector& p) = 0;
	virtual void		SetErrors(const JVector& p) = 0;
	virtual JFloat		FunctionN(const JFloat x) = 0;
	virtual JFloat		FunctionNPrimed(const JFloat x);
	
	void				SetFunctionString(const JString& str);

private:

	void				JPlotFitBaseX(J2DPlotWidget* plot, 
										J2DPlotDataBase* fitData);

	void				AdjustDiffData();
	JFloat				CalcError(const JVector& parameters, const JIndex constIndex);

private:

	JFloat	itsChi2;
	JFloat 	itsRangeXMax;
	JFloat	itsRangeXMin;
	JFloat 	itsRangeYMax;
	JFloat 	itsRangeYMin;
	bool 	itsUsingRange;
	JSize	itsRealCount;
	
	JArray<J2DDataPoint>* itsRealData;

	JIndex		itsCurrentConstantParmIndex;
	JFloat		itsCurrentConstantParm;
	bool		itsUseAltFunction;
	JFloat		itsChiPlus;

	JString		itsFunctionStr;

};

#endif
