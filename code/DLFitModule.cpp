/******************************************************************************
 DLFitModule.cpp

	BASE CLASS = <NONE>

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include <DLFitModule.h>

#include <jx-af/jcore/JVector.h>

#include <ace/DLL.h>
#include <jx-af/jcore/jAssert.h>

using GetParmsFn     = const JUtf8Byte** (*)();
using GetParmCountFn = JSize (*)();
using GetNameFn      = const JUtf8Byte* (*)();

const JUtf8Byte* kInitValName	= "GetStartValues";
const JUtf8Byte* kFNName		= "FunctionN";
const JUtf8Byte* kFNPrimedName	= "FunctionNPrimed";
const JUtf8Byte* kGetParmsName	= "GetParms";
const JUtf8Byte* kParmCountName	= "GetParmCount";
const JUtf8Byte* kFormName		= "GetFunctionForm";
const JUtf8Byte* kFitName		= "GetFitName";

/******************************************************************************
 Create & Constructor

 *****************************************************************************/

bool
DLFitModule::Create
	(
	const JString&	moduleName,
	DLFitModule**	fit
	)
{
	ACE_DLL* module = jnew ACE_DLL(moduleName.GetBytes());
	assert(module != nullptr);

	bool ok	= true;

	EvalFn	fn = (EvalFn)module->symbol(kFNName);
	if (fn == nullptr)
	{
		ok	= false;
	}
	EvalFn	fnprimed = (EvalFn)module->symbol(kFNPrimedName);
	GetParmsFn pf    = (GetParmsFn)module->symbol(kGetParmsName);
	if (pf == nullptr)
	{
		ok	= false;
	}
	GetParmCountFn pc = (GetParmCountFn)module->symbol(kParmCountName);
	if (pc == nullptr)
	{
		ok	= false;
	}
	GetNameFn ff = (GetNameFn)module->symbol(kFormName);
	if (ff == nullptr)
	{
		ok	= false;
	}
	GetNameFn fname	= (GetNameFn)module->symbol(kFitName);
	if (fname == nullptr)
	{
		ok	= false;
	}
	InitialValFn ifn = (InitialValFn)module->symbol(kInitValName);

	if (!ok)
	{
		jdelete module;
		return false;
	}

	*fit = jnew DLFitModule(module, fn, fnprimed, ifn, pc(), pf(), ff(), fname());
	assert(*fit != nullptr);
	return true;
}

DLFitModule::DLFitModule
	(
	ACE_DLL*			module,
	EvalFn				function,
	EvalFn				fprimed,
	InitialValFn		initFn,
	const JSize		count,
	const JUtf8Byte**	parms,
	const JUtf8Byte*	form,
	const JUtf8Byte*	name
	)
	:
	itsFunctionalForm(form),
	itsFitName(name),
	itsParmCount(count),
	itsFunction(function),
	itsFPrimed(fprimed),
	itsGetStartValFn(initFn),
	itsModule(module)
{
	itsParmNames	= jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert(itsParmNames != nullptr);
	for (JIndex i = 1; i <= count; i++)
	{
		JString* str	= jnew JString(parms[i - 1]);
		itsParmNames->Append(str);
	}
	itsParameters	= jnew JVector(count);
}


/******************************************************************************
 Destructor

 *****************************************************************************/

DLFitModule::~DLFitModule()
{
	itsParmNames->DeleteAll();
	jdelete itsParmNames;
	jdelete itsModule;
	jdelete itsParameters;
}

/******************************************************************************
 HasStartValues (public)

 ******************************************************************************/

bool
DLFitModule::HasStartValues()
	const
{
	return itsGetStartValFn != nullptr;
}

/******************************************************************************
 GetStartValues (public)

	The arrays need to be pointers to allow for nullptr.

 ******************************************************************************/

bool
DLFitModule::GetStartValues
	(
	const JArray<JFloat>* x,
	const JArray<JFloat>* y,
	const JArray<JFloat>* xerr,
	const JArray<JFloat>* yerr,
	JVector*			  p
	)
{
	if (itsGetStartValFn == nullptr)
	{
		return false;
	}

	assert(x != nullptr);
	assert(y != nullptr);



//	itsGetStartValFn(x->GetElementCount(),

	return true;
}


/******************************************************************************
 Function (public)

 ******************************************************************************/

JFloat
DLFitModule::Function
	(
	const JFloat	x
	)
{
	return itsFunction(itsParameters->GetElements(), x);
}

/******************************************************************************
 HasFPrimed (public)

 ******************************************************************************/

bool
DLFitModule::HasFPrimed()
	const
{
	return itsFPrimed != nullptr;
}

/******************************************************************************
 FPrimed (public)

 ******************************************************************************/

JFloat
DLFitModule::FPrimed
	(
	const JFloat	x
	)
{
	assert(itsFPrimed != nullptr);
	return itsFPrimed(itsParameters->GetElements(), x);
}

/******************************************************************************
 SetCurrentParameters (public)

 ******************************************************************************/

void
DLFitModule::SetCurrentParameters
	(
	const JVector&	p
	)
{
	assert(p.GetDimensionCount() == itsParameters->GetDimensionCount());
	*itsParameters	= p;
}

/******************************************************************************
 GetParameterCount (public)

 ******************************************************************************/

JSize
DLFitModule::GetParameterCount()
	const
{
	return itsParmCount;
}

/******************************************************************************
 GetParameterName (public)

 ******************************************************************************/

const JString&
DLFitModule::GetParameterName
	(
	const JIndex index
	)
	const
{
	return *(itsParmNames->GetElement(index));
}

/******************************************************************************
 GetFunctionalForm (public)

 ******************************************************************************/

const JString&
DLFitModule::GetFunctionalForm()
	const
{
	return itsFunctionalForm;
}

/******************************************************************************
 GetFitName (public)

 ******************************************************************************/

const JString&
DLFitModule::GetFitName()
	const
{
	return itsFitName;
}
