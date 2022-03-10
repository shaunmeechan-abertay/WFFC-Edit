#pragma once
#include "DisplayObject.h"
class Commands
{
	//We don't know what objects are atm so just pretend this is an object
	//DisplayObject object;

public:
	enum class CommandType
	{
		Default,
		Delete,
		Create
	};

	CommandType type = CommandType::Default;

	//Can't think of an action that wouldn't involve an object.
	//Maybe terrain based actions?
	virtual void performAction(DisplayObject);
	virtual DisplayObject getDeletedObject();
	virtual CommandType getType();
};

