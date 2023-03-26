/******************************************************************************
 FitManager.h

	Interface for the FitManager class

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_FitManager
#define _H_FitManager

#include "FitDescription.h"
#include <jx-af/jcore/JPrefObject.h>
#include <jx-af/jcore/JBroadcaster.h>
#include <jx-af/jcore/JPtrArray.h>

class FitManager : public JPrefObject, virtual public JBroadcaster
{
public:

public:

	FitManager();
	~FitManager() override;

	JSize					GetFitCount() const;
	const FitDescription&	GetFitDescription(const JIndex index) const;
	FitDescription&			GetFitDescription(const JIndex index);

	void	AddFitDescription(FitDescription* fit);
	void	RemoveFitDescription(const JIndex index);

	bool	FitIsRemovable(const JIndex index);

protected:

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

private:

	JPtrArray<FitDescription>* itsFitDescriptions;

	bool	itsIsInitialized;

private:

	void	InitializeList();

public:

	static const JUtf8Byte* kFitsChanged;

	class FitsChanged : public JBroadcaster::Message
		{
		public:

			FitsChanged()
				:
				JBroadcaster::Message(kFitsChanged)
				{ };
		};
};

#endif
