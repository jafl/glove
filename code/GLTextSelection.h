/******************************************************************************
 GLTextSelection.h

	Interface for the GLTextSelection class

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_GLTextSelection
#define _H_GLTextSelection


#include <jx-af/jx/JXTextSelection.h>
#include <jx-af/jcore/JString.h>


class GLTextSelection : public JXTextSelection
{
public:

	GLTextSelection(JXDisplay* display, const std::string& text);
	virtual ~GLTextSelection();

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
