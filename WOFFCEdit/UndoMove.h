#pragma once
#include "Commands.h"
class UndoMove : public Commands
{
public:
	UndoMove();
	//Setup for a single object
	//This takes in the objects ORIGINAL position e.g. where it was before the drag
	void setup(int objectID, DirectX::XMVECTOR originalPosition, DirectX::XMVECTOR currentPosition);
	//Setup for multiple objects
	void setup(std::vector<int> objectsIDs, std::vector<DirectX::XMVECTOR> originalPositions, std::vector<DirectX::XMVECTOR> currentPositions);
	//Perform action takes in the displayList
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

