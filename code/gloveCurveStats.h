/*********************************************************************************
 gloveCurveStats.h

	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_CurveStats
#define _H_CurveStats

#include <jx-af/jcore/jTypes.h>

enum GCurveType
{
	kGDataCurve = 1,
	kGFunctionCurve,
	kGFitCurve,
	kGDiffCurve
};

enum GCurveFitType
{
	kLinearFit = 1,
	kGQuadFit,
	kGExpFit,
	kGPowFit,
	kGModFit,
	kGProxyFit
};

struct GloveCurveStats
{
	GCurveType		type;
	JIndex			provider;
	JIndex			fitNumber;
	GCurveFitType	fitType;
};

#endif
