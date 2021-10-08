/*********************************************************************************
 FitBase.h

	Interface for the FitBase class.

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_FitBase
#define _H_FitBase

#include <jx-af/jcore/jTypes.h>

class JString;

class FitBase
{
public:

	FitBase();
	FitBase(const JSize paramCount, const bool errors = false, const bool gof = false);

	virtual ~FitBase();

	JSize			GetParameterCount() const;
	virtual bool	GetParameterName(const JIndex index, JString* name) const = 0;
	virtual bool	GetParameter(const JIndex index, JFloat* value) const = 0;

	bool			HasParameterErrors() const;
	virtual bool	GetParameterError(const JIndex index, JFloat* value) const;

	bool			HasGoodnessOfFit() const;
	virtual bool	GetGoodnessOfFitName(JString* name) const;
	virtual bool	GetGoodnessOfFit(JFloat* value) const;

	virtual JString	GetFitFunctionString() const = 0;

protected:

	void	SetHasParameterErrors(const bool errors);
	void	SetParameterCount(const JSize count);
	void	SetHasGoodnessOfFit(const bool gof);

private:

	JSize	itsParameterCount;
	bool	itsHasParameterErrors;
	bool	itsHasGOF;

private:

	// not allowed

	FitBase(const FitBase&) = delete;
	FitBase& operator=(const FitBase&) = delete;
};

/*********************************************************************************
 HasParameterErrors

 ********************************************************************************/

inline bool
FitBase::HasParameterErrors()
	const
{
	return itsHasParameterErrors;
}

/*********************************************************************************
 GetParameterError

 ********************************************************************************/

inline bool
FitBase::GetParameterError
	(
	const JIndex index,
	JFloat* value
	)
	const
{
	return false;
}


/*********************************************************************************
 HasGoodnessOfFit

 ********************************************************************************/

inline bool
FitBase::HasGoodnessOfFit()
	const
{
	return itsHasGOF;
}

/*********************************************************************************
 GetGoodnessOfFitName

 ********************************************************************************/

inline bool
FitBase::GetGoodnessOfFitName
	(
	JString* name
	)
	const
{
	return false;
}

/*********************************************************************************
 GetGoodnessOfFit

 ********************************************************************************/

inline bool
FitBase::GetGoodnessOfFit
	(
	JFloat* value
	)
	const
{
	return false;
}

/*********************************************************************************
 GetParameterCount

 ********************************************************************************/

inline JSize
FitBase::GetParameterCount()
	const
{
	return itsParameterCount;
}

#endif
