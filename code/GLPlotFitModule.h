/******************************************************************************
 GLPlotFitModule.h

	Interface for the GLPlotFitModule class

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GLPlotFitModule
#define _H_GLPlotFitModule

// Superclass Header
#include <GLPlotFitBase.h>
#include <JString.h>

class GLDLFitModule;
class JVector;

class GLPlotFitModule : public GLPlotFitBase
{
public:

	GLPlotFitModule(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					  const JFloat xMin, const JFloat xMax);
	GLPlotFitModule(J2DPlotWidget* plot, J2DPlotDataBase* fitData, 
					  const JFloat xmin, const JFloat xmax,
					  const JFloat ymin, const JFloat ymax);
	virtual ~GLPlotFitModule();

	void	SetFitModule(GLDLFitModule* fit);
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

	GLDLFitModule*	itsModule;			// we don't own this
	JVector*		itsErrors;
	JVector*		itsParameters;
};
#endif
