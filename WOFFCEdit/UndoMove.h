#pragma once
#include "Commands.h"
class UndoMove : public Commands
{
public:
	UndoMove();
	//Setup for a single object
	void setup(DisplayObject* object, DirectX::XMVECTOR originalPosition);
	//Setup for multiple objects
	void setup(std::vector<DisplayObject*> objects, std::vector<DirectX::XMVECTOR> originalPositions);
	//Perform action for objects
	void performAction();
	//Getters
	DisplayObject* getMovedObject();
	std::vector<DisplayObject*> getMovedObjects();
	//These will be used to undo this command
	DirectX::XMVECTOR getMovedObjectsPreviousPosition();
	std::vector<DirectX::XMVECTOR> getMovedObjectsPreviousPositions();
private:
	DisplayObject* movedObject;
	DirectX::XMVECTOR movedObjectsOriginalPosition;
	DirectX::XMVECTOR movedObjectsPreviousPosition;
	std::vector<DisplayObject*> movedObjects;
	std::vector<DirectX::XMVECTOR> movedObjectsOriginalPositions;
	std::vector<DirectX::XMVECTOR> movedObjectsPreviousPositions;
};

