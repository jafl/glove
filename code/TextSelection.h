/******************************************************************************
 TextSelection.h

	Interface for the TextSelection class

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_TextSelection
#define _H_TextSelection


#include <jx-af/jx/JXTextSelection.h>
#include <jx-af/jcore/JString.h>


class TextSelection : public JXTextSelection
{
public:

	TextSelection(JXDisplay* display, const std::string& text);
	virtual ~TextSelection();

	void	SetGloveData(const std::string& text);

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual bool	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const;

private:

	Atom	itsGloveTextXAtom;
	JString	itsGloveData;
};

#endif
