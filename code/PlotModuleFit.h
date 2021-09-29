/*********************************************************************************
 PlotModuleFit.h
 
	Interface for the PlotModuleFit class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_PlotModuleFit
#define _H_PlotModuleFit

#include <PlotFitFunction.h>
#include <jx-af/jcore/JPtrArray-JString.h>

class J2DPlotWidget;
class JFunction;
class VarList;

class PlotModuleFit : public PlotFitFunction
{
public:

	PlotModuleFit(	J2DPlotWidget* plot, J2DPlotDataBase* fitData,
					const JFloat xMin, const JFloat xMax,
					JPtrArray<JString>* names, JArray<JFloat>* values,
					JFunction*	function,
					VarList* list,
					const JSize parmscount,	
					const bool errors = false, 
					const bool gof = false);
	PlotModuleFit(J2DPlotWidget* plot, J2DPlotDataBase* fitData, 
					JPtrArray<JString>* names, JArray<JFloat>* values,
					JFunction*	function,
					VarList* list,
					const JFloat xmin, const JFloat xmax,
					const JFloat ymin, const JFloat ymax,
					const JSize parmscount,	
					const bool errors = false, 
					const bool gof = false);
	PlotModuleFit(J2DPlotWidget* plot, J2DPlotDataBase* fitData, std::istream& is);
	virtual ~PlotModuleFit();	

	virtual void GetElement(const JIndex index, J2DDataPoint* data) const override;

	virtual	void UpdateFunction(const JFloat xmin, const JFloat xmax, 
								const JSize steps);

	virtual bool	GetParameterName(const JIndex index, JString* name) const override;
	virtual bool	GetParameter(const JIndex index, JFloat* value) const override;

	virtual bool	GetParameterError(const JIndex index, JFloat* value) const override;

	virtual bool	GetGoodnessOfFitName(JString* name) const override;
	virtual bool	GetGoodnessOfFit(JFloat* value) const override;
	
	virtual JString		GetFunctionString() const override;
	virtual JString		GetFitFunctionString() const override;

	virtual bool	GetYValue(const JFloat x, JFloat* y) const override;
	virtual const J2DPlotDataBase*		GetDataToFit() const;
	
	void				WriteData(std::ostream& os);


protected:

	JFloat				GetCurrentXMax() const;
	JFloat				GetCurrentXMin() const;
	JFloat				GetCurrentStepCount() const;
	virtual bool	DataElementValid(const JIndex index) override;
	virtual bool	GetDataElement(const JIndex index, J2DDataPoint* point) override;

private:

	void		JPlotModuleFitX(J2DPlotWidget* plot, 
								J2DPlotDataBase* fitData, 
								JPtrArray<JString>* names, 
								JArray<JFloat>* values,
								JFunction*	function,
								VarList* list,
								const JSize parmscount,	
								const bool errors = false, 
								const bool gof = false);

private:

	JString		itsFunctionName;
	JFloat		itsCurrentXMin;
	JFloat		itsCurrentXMax;
	JSize		itsCurrentStepCount;
	JFloat 		itsRangeXMax;
	JFloat		itsRangeXMin;
	JFloat 		itsRangeYMax;
	JFloat 		itsRangeYMin;
	bool 	itsUsingRange;
	JPtrArray<JString>* itsNames;
	JArray<JFloat>* 	itsValues;
	JFunction*			itsFunction;
	VarList*			itsList;

};

/*********************************************************************************
 GetDataToFit
 

 ********************************************************************************/

inline const J2DPlotDataBase*
PlotModuleFit::GetDataToFit()
	const
{
	return GetData();
}

/*********************************************************************************
 GetCurrentXMax
 

 ********************************************************************************/

inline JFloat
PlotModuleFit::GetCurrentXMax()
	const
{
	return itsCurrentXMax;
}

/*********************************************************************************
 GetCurrentXMin
 

 ********************************************************************************/

inline JFloat
PlotModuleFit::GetCurrentXMin()
	const
{
	return itsCurrentXMin;
}

/*********************************************************************************
 GetCurrentStepCount
 

 ********************************************************************************/

inline JFloat
PlotModuleFit::GetCurrentStepCount()
	const
{
	return itsCurrentStepCount;
}


#endif
