/*********************************************************************************
 PlotFitLinearEq.h
 
	Interface for the PlotFitLinearEq class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_PlotFitLinearEq
#define _H_PlotFitLinearEq

#include <PlotFitBase.h>
#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/JString.h>

class J2DPlotWidget;
class JVector;
class JMatrix;

class PlotFitLinearEq : public PlotFitBase
{
public:

	PlotFitLinearEq(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	PlotFitLinearEq(J2DPlotWidget* plot, J2DPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~PlotFitLinearEq();

	void	InitializePolynomial(const JArray<JIndex>& powers);
	void	GenerateFit();

	virtual bool	GetParameterName(const JIndex index, JString* name) const;
	virtual bool	GetParameter(const JIndex index, JFloat* value) const;

	virtual bool	GetParameterError(const JIndex index, JFloat* value) const;

	virtual bool	GetYValue(const JFloat x, JFloat* y) const;

protected:

	virtual void		SetCurrentParameters(const JVector& p);
	virtual void		SetErrors(const JVector& p);
	virtual JFloat		FunctionN(const JFloat x);
	virtual JFloat		FunctionNPrimed(const JFloat x);


private:

	void				JPlotFitLinearEqX(J2DPlotWidget* plot, 
										  J2DPlotDataBase* fitData);

	void				CalculateFirstPass();

private:

	JArray<JIndex>*	itsPowers;
	JVector*		itsParameters;
	JVector*		itsErrors;
	
	JFloat		itsChi2Start;
	
};

#endif
