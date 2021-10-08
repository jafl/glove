/******************************************************************************
 PlotFitProxy.h

	Interface for the PlotFitProxy class

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_PlotFitProxy
#define _H_PlotFitProxy


#include <PlotFitFunction.h>

#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/JString.h>

class VarList;
class JFunction;

class PlotFitProxy : public PlotFitFunction
{
public:

public:

	PlotFitProxy(PlotFitFunction* fit,
				  J2DPlotWidget* plot, J2DPlotDataBase* fitData);
	PlotFitProxy(J2DPlotWidget* plot, J2DPlotDataBase* fitData, std::istream& is);
	~PlotFitProxy() override;

	void	WriteData(std::ostream& os);

	bool	GetParameterName(const JIndex index, JString* name) const override;
	bool	GetParameter(const JIndex index, JFloat* value) const override;

	bool	GetParameterError(const JIndex index, JFloat* value) const override;

	bool	GetGoodnessOfFitName(JString* name) const override;
	bool	GetGoodnessOfFit(JFloat* value) const override;

	JString	GetFitFunctionString() const override;

	JString	GetFunctionString() const override;
	bool	GetYValue(const JFloat x, JFloat* y) const override;

protected:

private:

	VarList*		itsParms;
	JArray<JFloat>*	itsErrors;
	bool			itsHasGOF;
	JString			itsGOFName;
	JFloat			itsGOF;
	JString			itsFnString;
	JFunction*		itsFn;
};

#endif
