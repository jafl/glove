/******************************************************************************
 GLPlotFitNonLinear.h

	Interface for the GLPlotFitNonLinear class

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GLPlotFitNonLinear
#define _H_GLPlotFitNonLinear

// Superclass Header
#include <GLPlotFitBase.h>
#include <jx-af/jcore/JString.h>

class GLVarList;
class JFunction;
class JVector;

class GLPlotFitNonLinear : public GLPlotFitBase
{
public:

	GLPlotFitNonLinear(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					  const JFloat xMin, const JFloat xMax);
	GLPlotFitNonLinear(J2DPlotWidget* plot, J2DPlotDataBase* fitData, 
					  const JFloat xmin, const JFloat xmax,
					  const JFloat ymin, const JFloat ymax);
	virtual ~GLPlotFitNonLinear();

	void	SetVarList(GLVarList* varList);
	void	SetFunction(const JString& function);
	void	SetFPrimed(const JString& fPrimed);
	void	SetInitialParameters(const JVector& p);

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

	GLVarList*		itsVarList;			// we don't own this
	JFunction*		itsFunction;
	JFunction*		itsFPrimed;
	JVector*		itsErrors;
};

#endif
