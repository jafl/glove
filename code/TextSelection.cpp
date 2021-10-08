/******************************************************************************
 TextSelection.cpp

	BASE CLASS = public JXTextSelection

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include "TextSelection.h"
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kGloveTextDataXAtomName = "OVE_TEXT";

/******************************************************************************
 Constructor

 *****************************************************************************/

TextSelection::TextSelection
	(
	JXDisplay*			display,
	const std::string&	text
	)
	:
	JXTextSelection(display, JString(text.c_str(), text.length(), JString::kNoCopy), nullptr)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

TextSelection::~TextSelection()
{
}

/******************************************************************************
 SetGloveData (public)

 ******************************************************************************/

void
TextSelection::SetGloveData
	(
	const std::string& text
	)
{
	itsGloveData = text;
}

/******************************************************************************


 ******************************************************************************/

void
TextSelection::AddTypes
	(
	const Atom selectionName
	)
{
	itsGloveTextXAtom = AddType(kGloveTextDataXAtomName);
	JXTextSelection::AddTypes(selectionName);
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

bool
TextSelection::ConvertData
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{

	if (requestType == itsGloveTextXAtom)
	{
		*bitsPerBlock = 8;
		*data = jnew unsigned char[ itsGloveData.GetByteCount()];
		assert( *data != nullptr );
		memcpy(*data, itsGloveData.GetRawBytes(), itsGloveData.GetByteCount());
		*dataLength = itsGloveData.GetByteCount();
		*returnType = itsGloveTextXAtom;
		return true;
	}
	return JXTextSelection::ConvertData(requestType, returnType, data, dataLength, bitsPerBlock);
}
