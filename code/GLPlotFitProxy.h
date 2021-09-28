/******************************************************************************
 GLPlotFitProxy.h

	Interface for the GLPlotFitProxy class

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_GLPlotFitProxy
#define _H_GLPlotFitProxy


#include <GLPlotFitFunction.h>

#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/JString.h>

class GLVarList;
class JFunction;

class GLPlotFitProxy : public GLPlotFitFunction
{
public:

public:

	GLPlotFitProxy(GLPlotFitFunction* fit,
				  J2DPlotWidget* plot, J2DPlotDataBase* fitData);
	GLPlotFitProxy(J2DPlotWidget* plot, J2DPlotDataBase* fitData, std::istream& is);
	virtual ~GLPlotFitProxy();

	void	WriteData(std::ostream& os);

	virtual bool	GetParameterName(const JIndex index, JString* name) const;
	virtual bool	GetParameter(const JIndex index, JFloat* value) const;

	virtual bool	GetParameterError(const JIndex index, JFloat* value) const;

	virtual bool	GetGoodnessOfFitName(JString* name) const;
	virtual bool	GetGoodnessOfFit(JFloat* value) const;
	
	virtual JString	GetFitFunctionString() const;

	virtual JString	GetFunctionString() const;
	virtual bool	GetYValue(const JFloat x, JFloat* y) const;

protected:

private:

	GLVarList*		itsParms;
	JArray<JFloat>*	itsErrors;
	bool			itsHasGOF;
	JString			itsGOFName;
	JFloat			itsGOF;
	JString			itsFnString;
	JFunction*		itsFn;
};

#endif
