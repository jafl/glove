/******************************************************************************
 DLFitModule.h

	Interface for the DLFitModule class

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_DLFitModule
#define _H_DLFitModule

#include <jx-af/jcore/jTypes.h>
#include <jx-af/jcore/JPtrArray-JString.h>

using EvalFn = JFloat (*)(const JFloat*, const JFloat);
using InitialValFn = JFloat (*)(const JSize, const JFloat*, const JFloat*, const JFloat*, const JFloat*, JFloat*);

class ACE_DLL;
class JVector;

class DLFitModule
{
public:

	static bool Create(const JString& moduleName, DLFitModule** fit);
	~DLFitModule();

	bool	HasStartValues() const;
	bool	GetStartValues(const JArray<JFloat>* x, const JArray<JFloat>* y,
						   const JArray<JFloat>* xerr, const JArray<JFloat>* yerr,
						   JVector* p);
	JFloat	Function(const JFloat x);
	JFloat	FPrimed(const JFloat x);
	bool	HasFPrimed() const;
	void	SetCurrentParameters(const JVector& p);

	JSize			GetParameterCount() const;
	const JString&	GetParameterName(const JIndex index) const;

	const JString&	GetFunctionalForm() const;
	const JString&	GetFitName() const;

protected:

	DLFitModule(ACE_DLL* module, EvalFn function, EvalFn fprimed,
				InitialValFn initFn,
				const JSize count, const JUtf8Byte** parms,
				const JUtf8Byte* form, const JUtf8Byte* name);

private:

	JString				itsFunctionalForm;
	JString				itsFitName;
	JSize				itsParmCount;
	JPtrArray<JString>*	itsParmNames;
	JVector*			itsParameters;

	EvalFn			itsFunction;
	EvalFn			itsFPrimed;
	InitialValFn	itsGetStartValFn;

	ACE_DLL*	itsModule;

//	JFloat		(*itsEvaluateFn)(const JFloat*, const JFloat);
//	JFloat		(*itsEvaluateDerivativeFn)(const JFloat*, const JFloat);

private:

	// not allowed

	DLFitModule(const DLFitModule&) = delete;
	DLFitModule& operator=(const DLFitModule&) = delete;
};

#endif
