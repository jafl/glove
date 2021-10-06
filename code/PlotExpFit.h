/*********************************************************************************
 PlotExpFit.h
 
	Interface for the PlotExpFit class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_PlotExpFit
#define _H_PlotExpFit

#include <PlotLinearFit.h>
#include <jx-af/jcore/jTypes.h>
#include <jx-af/jcore/JArray.h>

class J2DPlotWidget;
class JString;

class PlotExpFit : public PlotLinearFit
{
public:

	PlotExpFit(J2DPlotWidget* plot, J2DPlotDataBase* fitData,
				const JFloat xMin, const JFloat xMax);
	virtual ~PlotExpFit();	

	bool	GetYRange(const JFloat xMin, const JFloat xMax,
								  const bool xLinear,
								  JFloat* yMin, JFloat* yMax) const override;

	bool	GetYValue(const JFloat x, JFloat* y) const override;
	const J2DPlotDataBase*		GetDataToFit() const override;

	bool	GetParameter(const JIndex index, JFloat* value) const override;
	bool	GetParameterError(const JIndex index, JFloat* value) const override;
//	virtual bool	GetGoodnessOfFit(JFloat* value) const;

private:

	J2DPlotData*		itsAdjustedData;
	JArray<JFloat>*		itsXData;
	JArray<JFloat>*		itsYData;
	JArray<JFloat>*		itsXErrData;
	JArray<JFloat>*		itsYErrData;
	bool			itsHasXErrors;
	bool			itsHasYErrors;
	
private:

	void					AdjustData();
	void					CreateData();
	void					AdjustDiffData();
		
};

#endif
