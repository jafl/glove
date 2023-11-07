/******************************************************************************
 VarList.cpp

							The VarList Class

	BASE CLASS = JVariableList

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "VarList.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JStringMatch.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

	file format:
	{
		N <name> <value>
		N <name>[array size] <values>
	}
		*

 ******************************************************************************/

VarList::VarList()
	:
	JVariableList()
{
	GVarListX();
}

VarList::VarList
	(
	const VarList& list
	)
	:
	JVariableList()
{
	GVarListX();

	*itsValues	= *(list.itsValues);

	const JSize ncount	= list.itsNames->GetItemCount();
	for (JIndex i = 1; i <= ncount; i++)
	{
		JString* str = jnew JString(*(list.itsNames->GetItem(i)));
		assert(str != nullptr);
		itsNames->Append(str);
	}

	const JSize acount	= list.itsArrays->GetItemCount();
	for (JIndex i = 1; i <= acount; i++)
	{
		JArray<JFloat>* array = list.itsArrays->GetItem(i);
		if (array != nullptr)
		{
			array	= jnew JArray<JFloat>(*array);
			assert(array != nullptr);
		}
		itsArrays->Append(array);
	}
}

VarList::VarList
	(
	std::istream& input
	)
	:
	JVariableList()
{
	GVarListX();

	input >> std::ws;
	while (input.peek() != '*')
	{
		JUtf8Byte type;
		input >> type >> std::ws;
		if (type == 'N')
		{
			JString name = JReadUntilws(input);
			if (name.GetLastCharacter() != ']')
			{
				JFloat value;
				input >> value;
				AddVariable(name, value);
			}
			else
			{
				JStringIterator iter(&name);
				const bool foundBracket = iter.Next("[");
				assert( foundBracket && !iter.AtEnd() );

				iter.BeginMatch();
				iter.Next("]");
				const JString sizeStr = iter.FinishMatch().GetString();
				iter.SkipPrev(2 + sizeStr.GetCharacterCount());
				iter.RemoveAllNext();
				iter.Invalidate();

				JFloat x;
				const bool isNumber = sizeStr.ConvertToFloat(&x);
				assert( isNumber );
				const JSize arraySize = JRound(x);
				GNArray values(arraySize);
				for (JIndex i=1; i<= arraySize; i++)
				{
					JFloat value;
					input >> value;
					values.AppendItem(value);
				}
				AddArray(name, values);
			}
		}
		else
		{
			const JUtf8Character typeStr(type);
			const JUtf8Byte* map[] =
			{
				"name", typeStr.GetBytes()
			};
			const JString errorStr = JGetString("UnsupportedVariable::VarList", map, sizeof(map));
			JGetUserNotification()->ReportError(errorStr);
			JIgnoreUntil(input, '\n');
		}
		input >> std::ws;
	}
	JIgnoreUntil(input, '\n');
}

// private

void
VarList::GVarListX()
{
	itsNames = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsNames != nullptr );

	itsValues = jnew JArray<JFloat>;
	assert( itsValues != nullptr );

	itsArrays = jnew JPtrArray<GNArray>(JPtrArrayT::kDeleteAll);
	assert( itsArrays != nullptr );

	InstallCollection(itsNames);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

VarList::~VarList()
{
	jdelete itsNames;
	jdelete itsValues;
	jdelete itsArrays;
}

/******************************************************************************
 AddVariable

 ******************************************************************************/

bool
VarList::AddVariable
	(
	const JString&	name,
	const JFloat	value
	)
{
	JIndex index;
	if (NameValid(name) && !ParseVariableName(name, &index))
	{
		JString* varName = jnew JString(name);
		itsNames->Append(varName);
		itsValues->AppendItem(value);
		itsArrays->AppendItem(static_cast<GNArray*>(nullptr));
		return true;
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("NameUsed::VarList"));
		return false;
	}
}

/******************************************************************************
 RemoveVariable

 ******************************************************************************/

void
VarList::RemoveVariable
	(
	const JIndex index
	)
{
	assert(index <= itsNames->GetItemCount());
	itsNames->DeleteItem(index);
	itsValues->RemoveItem(index);
	itsArrays->RemoveItem(index);
}


/******************************************************************************
 AddArray

 ******************************************************************************/

bool
VarList::AddArray
	(
	const JString&	name,
	const GNArray&	values
	)
{
	if (NameValid(name))
	{
		JString* varName = jnew JString(name);
		itsNames->Append(varName);
		itsValues->AppendItem(0.0);
		GNArray* newArray = jnew GNArray(values);
		itsArrays->AppendItem(newArray);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 SetValue

 ******************************************************************************/

bool
VarList::SetValue
	(
	const JIndex	index,
	const JFloat	value
	)
{
	if (IsVariable(index))
	{
		const JFloat oldValue = itsValues->GetItem(index);
		if (value != oldValue)
		{
			itsValues->SetItem(index, value);
			Broadcast(JVariableList::VarValueChanged(index,1));
		}
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 SetNumericValue

 ******************************************************************************/

void
VarList::SetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	itemIndex,
	const JFloat	value
	)
{
	if (IsArray(variableIndex))
	{
		GNArray* values = itsArrays->GetItem(variableIndex);
		const JFloat oldValue = values->GetItem(itemIndex);
		if (value != oldValue)
		{
			values->SetItem(itemIndex, value);
			Broadcast(JVariableList::VarValueChanged(variableIndex,itemIndex));
		}
	}
	else
	{
		SetValue(variableIndex, value);
	}
}

void
VarList::SetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	itemIndex,
	const JComplex& value
	)
{
}

/******************************************************************************
 Virtual functions for VarList class

	Not inline because they are virtual

 ******************************************************************************/

/******************************************************************************
 GetVariableName

 ******************************************************************************/

const JString&
VarList::GetVariableName
	(
	const JIndex index
	)
	const
{
	return *(itsNames->GetItem(index));
}

void
VarList::GetVariableName
	(
	const JIndex	index,
	JString*		name,
	JString*		subscript
	)
	const
{
	JPtrArray<JString> s(JPtrArrayT::kDeleteAll);
	itsNames->GetItem(index)->Split("_", &s, 2);
	if (s.GetItemCount() == 2)
	{
		*name = *s.GetItem(1);
		*subscript = *s.GetItem(2);
	}
	else
	{
		*name = *s.GetItem(1);
		subscript->Clear();
	}
}

/******************************************************************************
 SetVariableName (public)

 ******************************************************************************/

bool
VarList::SetVariableName
	(
	const JIndex	varIndex,
	const JString&	str
	)
{
	JIndex index;
	if (!NameValid(str))
	{
		return false;
	}
	else if (ParseVariableName(str, &index) && index != varIndex)
	{
		JGetUserNotification()->ReportError(JGetString("NameUsed::VarList"));
		return false;
	}
	else
	{
		JString* varName = itsNames->GetItem(varIndex);
		*varName = str;
		Broadcast(VarNameChanged(varIndex));
		return true;
	}
}

/******************************************************************************
 IsArray

 ******************************************************************************/

bool
VarList::IsArray
	(
	const JIndex index
	)
	const
{
	return itsArrays->GetItem(index) != nullptr;
}

/******************************************************************************
 ArrayIndexValid

 ******************************************************************************/

bool
VarList::ArrayIndexValid
	(
	const JIndex variableIndex,
	const JIndex itemIndex
	)
	const
{
	return itemIndex == 1 ||
			(IsArray(variableIndex) &&
			 (itsArrays->GetItem(variableIndex))->IndexValid(itemIndex));
}

/******************************************************************************
 GetNumericValue

 ******************************************************************************/

bool
VarList::GetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	itemIndex,
	JFloat*			value
	)
	const
{
	GNArray* values = itsArrays->GetItem(variableIndex);
	if (values == nullptr && itemIndex == 1)
	{
		*value = itsValues->GetItem(variableIndex);
		return true;
	}
	else if (values != nullptr && values->IndexValid(itemIndex))
	{
		*value = values->GetItem(itemIndex);
		return true;
	}
	else
	{
		return false;
	}
}

bool
VarList::GetNumericValue
	(
	const JIndex	variableIndex,
	const JIndex	itemIndex,
	JComplex*		value
	)
	const
{
	JFloat x;
	if (GetNumericValue(variableIndex, itemIndex, &x))
	{
		*value = x;
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 GetVariableCount

 ******************************************************************************/

JSize
VarList::GetVariableCount()
	const
{
	return itsValues->GetItemCount();
}
