/*********************************************************************************
 PlotFitQuad2.h

	Interface for the PlotFitQuad2 class.

	Copyright @ 2000 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_PlotFitQuad2
#define _H_PlotFitQuad2

#include <PlotFitBase.h>
#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/JString.h>

class J2DPlotWidget;
class JVector;
class JMatrix;

class PlotFitQuad2 : public PlotFitBase
{
public:

	PlotFitQuad2(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	PlotFitQuad2(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax);

	~PlotFitQuad2() override;

	bool	GetParameterName(const JIndex index, JString* name) const override;
	bool	GetParameter(const JIndex index, JFloat* value) const override;

	bool	GetParameterError(const JIndex index, JFloat* value) const override;

	JString	GetFunctionString() const override;
	JString	GetFitFunctionString() const override;

	bool	GetYValue(const JFloat x, JFloat* y) const override;

protected:

	void	SetCurrentParameters(const JVector& p) override;
	void	SetErrors(const JVector& p) override;
	JFloat	FunctionN(const JFloat x) override;
	JFloat	FunctionNPrimed(const JFloat x) override;


private:

	void	JPlotFitQuad2X();

	void	CalculateFirstPass();

private:

	JString	itsFunctionName;
	JFloat	itsAParameter;
	JFloat	itsAErrParameter;
	JFloat	itsBParameter;
	JFloat	itsBErrParameter;
	JFloat	itsCParameter;
	JFloat	itsCErrParameter;
	JFloat	itsChi2Start;

};

#endif
