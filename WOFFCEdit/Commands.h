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
		Create,
		Copy,
		Pase,
		UndoMove,
		Move
	};

	CommandType type = CommandType::Default;

	//Can't think of an action that wouldn't involve an object.
	//Maybe terrain based actions?
	virtual void performAction(DisplayObject);
	//Used in UndoMove
	virtual void performAction(std::vector<DisplayObject>*);
	virtual DisplayObject getDeletedObject();
	virtual std::vector<DisplayObject> getDeletedObjects();
	virtual DisplayObject getCreatedObject();
	virtual std::vector<DisplayObject*> getCreatedObjects();
	//Used for undo redo move
	virtual int getMovedObjectsID();
	virtual std::vector<int> getMovedObjectsIDs();
	virtual DirectX::XMVECTOR getMovedObjectsPreviousPosition();
	virtual std::vector<DirectX::XMVECTOR> getMovedObjectsPreviousPositions();
	//Used in all
	virtual CommandType getType();
};

