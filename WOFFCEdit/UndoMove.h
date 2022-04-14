#pragma once
#include "Commands.h"
class UndoMove : public Commands
{
public:
	UndoMove();
	//Setup for a single object
	//This takes in the objects ORIGINAL position e.g. where it was before the drag
	void setup(int objectID, DirectX::XMVECTOR originalPosition);
	//Setup for multiple objects
	void setup(std::vector<int> objectsIDs, std::vector<DirectX::XMVECTOR> originalPositions);
	//Perform action takes in the displayList
	void performAction(std::vector<DisplayObject>* displayList);
	//Getters
	int getMovedObjectsID();
	std::vector<int> getMovedObjectsIDs();
	//These will be used to undo this command
	DirectX::XMVECTOR getMovedObjectsOriginalPosition();
	std::vector<DirectX::XMVECTOR> getMovedObjectsOriginalPositions();
private:
	int movedObjectID;
	DirectX::XMVECTOR movedObjectsOriginalPosition;
	std::vector<int> movedObjectsIDs;
	std::vector<DirectX::XMVECTOR> movedObjectsOriginalPositions;
};

