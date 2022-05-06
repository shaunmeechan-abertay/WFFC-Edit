#pragma once
#include "Commands.h"
#include <vector>
class RedoManipulationCommand : public Commands
{
public:
	RedoManipulationCommand();
	//This copies in a display object (basically a copy command)
	void setup(DisplayObject* object);
	//This copies in a vector of display objects
	void setup(std::vector<DisplayObject*> objects);
	//Perform action takes in the displayList
	void performAction(std::vector<DisplayObject>* displayList);
	DisplayObject* getStoredObject();
	std::vector<DisplayObject*> getStoredObjects();

private:
	DisplayObject* storedObject;
	DisplayObject tempObject;
	std::vector<DisplayObject> tempObjects;
	std::vector<DisplayObject*> storedObjects;

};

