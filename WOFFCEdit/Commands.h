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
		Move,
		UndoManipulation,
		RedoManipulation
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
	virtual std::vector<DisplayObject> getCreatedObjects();
	virtual std::vector<DisplayObject*> getCreatedObjectsAsPointers();
	//Used for undo redo move
	virtual int getMovedObjectsID();
	virtual std::vector<int> getMovedObjectsIDs();
	virtual DirectX::XMVECTOR getMovedObjectsOriginalPosition();
	virtual std::vector<DirectX::XMVECTOR> getMovedObjectsOriginalPositions();
	//These are used for object manipulation
	virtual DisplayObject* getStoredObject();
	virtual std::vector<DisplayObject*> getStoredObjects();
	//Used in all
	virtual CommandType getType();
};

