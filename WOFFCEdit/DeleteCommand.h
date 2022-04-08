#pragma once
#include "Commands.h"
#include "DisplayObject.h"
#include <vector>

class DeleteCommand: public Commands
{
public:
	DeleteCommand();
	void performAction(std::vector<DisplayObject>&objects, std::vector<DisplayObject*>& ObjectsToDelete);
	void performAction(std::vector<DisplayObject>&objects, int ID);
	Commands::CommandType getType();
	DisplayObject getDeletedObject();
	std::vector<DisplayObject> getDeletedObjects();
private:
	DisplayObject deletedObject;
	std::vector<DisplayObject> deletedObjects;
};

