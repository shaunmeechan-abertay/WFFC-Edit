#pragma once
#include "Commands.h"
class MoveCommand : public Commands
{
public:
	MoveCommand();
	//Setup for single object
	//This takes in the objects postion after an Undo
	void setup(int objectID, DirectX::XMVECTOR originalPosition);
	//Setup for multiple
	void setup(std::vector<int>objectsIDs, std::vector<DirectX::XMVECTOR> originalPositons);
	//Perform action takes in displayList
	void performAction(std::vector<DisplayObject>* displayList);
	//Getters
	int getMovedObjectsID();
	std::vector<int> getMovedObjectsIDs();
	//These will be used to undo this command
	DirectX::XMVECTOR getMovedObjectsPreviousPosition();
	std::vector<DirectX::XMVECTOR> getMovedObjectsPreviousPositions();
private:
	int movedObjectID;
	DirectX::XMVECTOR movedObjectsOriginalPosition;
	DirectX::XMVECTOR movedObjectsPreviousPosition;
	std::vector<int> movedObjectsIDs;
	std::vector<DirectX::XMVECTOR> movedObjectsOriginalPositions;
	std::vector<DirectX::XMVECTOR> movedObjectsPreviousPositions;
};

