/*********************************************************************************
 GLPlotFitFunction.h
 
	Interface for the GLPlotFitFunction class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_GLPlotFitFunction
#define _H_GLPlotFitFunction

#include <J2DPlotFunctionBase.h>
#include <GLFitBase.h>
#include <JArray.h>

class J2DPlotWidget;
class J2DPlotData;

class GLPlotFitFunction : public J2DPlotFunctionBase, public GLFitBase
{
public:

	GLPlotFitFunction(J2DPlotWidget* plot, J2DPlotDataBase* fitData, 
						const JFloat xMin, const JFloat xMax);
	virtual ~GLPlotFitFunction();	

	const J2DPlotDataBase*	GetData() const;
	J2DPlotData*			GetDiffData() const;
	JFloat					GetStdDev() const;

protected:

	virtual void 	GenerateDiffData();
	void			CalculateStdDev();
	void			AdjustDiffDataValue(const JIndex index, const JFloat value);
	virtual bool	DataElementValid(const JIndex index);
	virtual bool	GetDataElement(const JIndex index, J2DDataPoint* point);
	void			SetDiffData(J2DPlotData* data);
	
private:

	J2DPlotDataBase*	itsData;
	J2DPlotData*	itsDiffData;
	bool			itsHasXErrors;
	bool			itsHasYErrors;
	JFloat			itsStdDev;
};

/*********************************************************************************
 GetData
 

 ********************************************************************************/
 
inline const J2DPlotDataBase*
GLPlotFitFunction::GetData()
	const
{
	return itsData;
}

/*********************************************************************************
 GetData
 

 ********************************************************************************/
 
inline JFloat
GLPlotFitFunction::GetStdDev()
	const
{
	return itsStdDev;
}


#endif
