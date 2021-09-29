/******************************************************************************
 VarList.h

	Interface for VarList class.

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GVarList
#define _H_GVarList

#include <jx-af/jexpr/JVariableList.h>
#include <jx-af/jcore/JPtrArray.h>

class JString;

typedef JArray<JFloat>	GNArray;

class VarList : public JVariableList
{
public:

	VarList();
	VarList(const VarList& list);
	VarList(std::istream& input);

	virtual ~VarList();

	bool	AddVariable(const JString& name, const JFloat value);
	void	RemoveVariable(const JIndex index);
	bool	AddArray(const JString& name, const GNArray& values);

	bool	IsVariable(const JIndex index) const;
	bool	SetValue(const JIndex index, const JFloat value);
	JSize	GetVariableCount() const;
	bool	SetElementValue(const JIndex variableIndex, 
							const JIndex elementIndex,
							const JFloat value);

	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JFloat value) override;
	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JComplex& value) override;
	
// implementation of JVariableList

	virtual const JString&	GetVariableName(const JIndex index) const override;
	virtual void			GetVariableName(const JIndex index, JString* name,
											JString* subscript) const override;
	bool					SetVariableName(const JIndex index, const JString& str);

	const JPtrArray<JString>&	GetVariables() const;

	virtual bool	IsArray(const JIndex index) const override;
	virtual bool	ArrayIndexValid(const JIndex variableIndex,
									const JIndex elementIndex) const override;

	virtual bool	GetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex, JFloat* value) const override;
	virtual bool	GetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex, JComplex* value) const override;

private:

	JPtrArray<JString>*		itsNames;
	JArray<JFloat>*			itsValues;
	JPtrArray<GNArray>*		itsArrays;

private:

	void	GVarListX();
};

/******************************************************************************
 IsVariable

 ******************************************************************************/

inline bool
VarList::IsVariable
	(
	const JIndex index
	)
	const
{
	return !(IsArray(index));
}

/******************************************************************************
 GetVariables (public)

 ******************************************************************************/

inline const JPtrArray<JString>&
VarList::GetVariables()
	const
{
	return *itsNames;
}

#endif
