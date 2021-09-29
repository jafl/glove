/*********************************************************************************
 PlotFitExp.h
 
	Interface for the PlotFitExp class.
 
	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_PlotFitExp
#define _H_PlotFitExp

#include <PlotFitBase.h>
#include <jx-af/jcore/JString.h>

class J2DPlotWidget;

class PlotFitExp : public PlotFitBase
{
public:

	PlotFitExp(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	PlotFitExp(J2DPlotWidget* plot, J2DPlotDataBase* fitData, 
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);
	virtual ~PlotFitExp();	

	void				GenerateFit();

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

	JFloat		itsAParm;
	JFloat		itsBParm;
	JFloat		itsAErr;
	JFloat		itsBErr;

	JFloat		itsChi2Start;

private:

	void	CalculateFirstPass();

	void	JPlotFitExpX(J2DPlotWidget* plot, J2DPlotDataBase* fitData);
};

#endif
