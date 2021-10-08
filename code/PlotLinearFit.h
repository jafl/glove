/*********************************************************************************
 PlotLinearFit.h

	Interface for the PlotLinearFit class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_PlotLinearFit
#define _H_PlotLinearFit

#include <PlotFitFunction.h>
#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/JString.h>

class J2DPlotWidget;

class PlotLinearFit : public PlotFitFunction
{
public:

	PlotLinearFit(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax,
					const bool xlog = false, const bool ylog = false);
	PlotLinearFit(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax,
					const bool xlog = false, const bool ylog = false);
	~PlotLinearFit() override;

//	virtual void	 GetElement(const JIndex index, J2DDataPoint* data) const;
	bool	GetYRange(const JFloat xMin, const JFloat xMax,
					  const bool xLinear,
					  JFloat* yMin, JFloat* yMax) const override;

//	virtual	void UpdateFunction(const JFloat xmin, const JFloat xmax,
//								const JSize steps);

	bool	GetParameterName(const JIndex index, JString* name) const override;
	bool	GetParameter(const JIndex index, JFloat* value) const override;

	bool	GetParameterError(const JIndex index, JFloat* value) const override;

	bool	GetGoodnessOfFitName(JString* name) const override;
	bool	GetGoodnessOfFit(JFloat* value) const override;

	JString	GetFunctionString() const override;
	JString	GetFitFunctionString() const override;

	bool	GetYValue(const JFloat x, JFloat* y) const override;

	virtual const J2DPlotDataBase*	GetDataToFit() const;

	void	AdjustDataRange(const JFloat xmin, const JFloat xmax,
							const JFloat ymin, const JFloat ymax);

protected:

	void			Paramin(JFloat ax, JFloat bx, JFloat cx, JFloat* xmin);
	virtual JFloat	ChiSqr(JFloat Bt);
	virtual JFloat	ChiSqrErr(JFloat Bt);
	JFloat			Root(JFloat xtemp);
	void			GenerateFit();
	void			SetFunctionName(const JString& name);
	JFloat			GetCurrentXMax() const;
	JFloat			GetCurrentXMin() const;
	JFloat			GetCurrentStepCount() const;
	bool			DataElementValid(const JIndex index) override;
	bool			GetDataElement(const JIndex index, J2DDataPoint* point) override;

private:

	void	JPlotLinearFitX(J2DPlotWidget* plot,
							J2DPlotDataBase* fitData,
							bool xlog, const bool ylog);

	void	LinearLSQ1();
	void	LinearLSQ2();
	void	Variance(JFloat* vx, JFloat* vy);

	void	AdjustDiffData();

private:

	JString		itsFunctionName;
	JFloat		itsCurrentXMin;
	JFloat		itsCurrentXMax;
	JSize		itsCurrentStepCount;
	JFloat		itsAParameter;
	JFloat		itsAErrParameter;
	JFloat		itsBParameter;
	JFloat		itsBErrParameter;
	JFloat		itsChi2;
	JFloat		itsAParameterT;
	JFloat		itsBParameterT;
	JFloat		itsChi2T;
	JFloat		itsRangeXMax;
	JFloat		itsRangeXMin;
	JFloat		itsRangeYMax;
	JFloat		itsRangeYMin;
	bool		itsUsingRange;
	bool		itsYIsLog;
	bool		itsXIsLog;
	JSize		itsRealCount;

};

/*********************************************************************************
 GetDataToFit


 ********************************************************************************/

inline const J2DPlotDataBase*
PlotLinearFit::GetDataToFit()
	const
{
	return GetData();
}

/*********************************************************************************
 GetCurrentXMax


 ********************************************************************************/

inline JFloat
PlotLinearFit::GetCurrentXMax()
	const
{
	return itsCurrentXMax;
}

/*********************************************************************************
 GetCurrentXMin


 ********************************************************************************/

inline JFloat
PlotLinearFit::GetCurrentXMin()
	const
{
	return itsCurrentXMin;
}

/*********************************************************************************
 GetCurrentStepCount


 ********************************************************************************/

inline JFloat
PlotLinearFit::GetCurrentStepCount()
	const
{
	return itsCurrentStepCount;
}


#endif
