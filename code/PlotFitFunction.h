/*********************************************************************************
 PlotFitFunction.h

	Interface for the PlotFitFunction class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_PlotFitFunction
#define _H_PlotFitFunction

#include <jx-af/j2dplot/J2DPlotFunctionBase.h>
#include "FitBase.h"
#include <jx-af/jcore/JArray.h>

class J2DPlotWidget;
class J2DPlotData;

class PlotFitFunction : public J2DPlotFunctionBase, public FitBase
{
public:

	PlotFitFunction(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax);
	~PlotFitFunction() override;

	const J2DPlotDataBase*	GetData() const;
	J2DPlotData*			GetDiffData() const;
	JFloat					GetStdDev() const;

protected:

	virtual void	GenerateDiffData();
	void			CalculateStdDev();
	void			AdjustDiffDataValue(const JIndex index, const JFloat value);
	virtual bool	DataElementValid(const JIndex index);
	virtual bool	GetDataElement(const JIndex index, J2DDataPoint* point);
	void			SetDiffData(J2DPlotData* data);

private:

	J2DPlotDataBase*	itsData;
	J2DPlotData*		itsDiffData;
	bool				itsHasXErrors;
	bool				itsHasYErrors;
	JFloat				itsStdDev;
};

/*********************************************************************************
 GetData

 ********************************************************************************/

inline const J2DPlotDataBase*
PlotFitFunction::GetData()
	const
{
	return itsData;
}

/*********************************************************************************
 GetData

 ********************************************************************************/

inline JFloat
PlotFitFunction::GetStdDev()
	const
{
	return itsStdDev;
}


#endif
