#include "UndoMove.h"

UndoMove::UndoMove()
{
	type = Commands::CommandType::UndoMove;
}

//This takes in the object ORIGINAL position e.g. where it was before the drag
void UndoMove::setup(int objectID, DirectX::XMVECTOR originalPosition)
{
	movedObjectID = objectID;
	movedObjectsOriginalPosition = originalPosition;
}

void UndoMove::setup(std::vector<int> objectsIDs, std::vector<DirectX::XMVECTOR> originalPositions)
{
	for (unsigned int i = 0; i < objectsIDs.size(); i++)
	{
		movedObjectsIDs.push_back(objectsIDs[i]);
		movedObjectsOriginalPositions.push_back(originalPositions[i]);
	}
}

void UndoMove::performAction(std::vector<DisplayObject>* displayList)
{
	if (movedObjectsIDs.empty() == true)
	{
		//Find the object
		for (unsigned int i = 0; i < displayList->size(); i++)
		{
			if (movedObjectID == displayList->at(i).m_ID)
			{
				displayList->at(i).m_position = movedObjectsOriginalPosition;
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < movedObjectsIDs.size(); i++)
		{
			for (unsigned int j = 0; j < displayList->size(); j++)
			{
				if (movedObjectsIDs[i] == displayList->at(j).m_ID)
				{
					displayList->at(j).m_position = movedObjectsOriginalPositions[i];
					break;
				}
			}
		}
	}
}

int UndoMove::getMovedObjectsID()
{
	return movedObjectID;
}

std::vector<int> UndoMove::getMovedObjectsIDs()
{
	return movedObjectsIDs;
}

DirectX::XMVECTOR UndoMove::getMovedObjectsOriginalPosition()
{
	return movedObjectsOriginalPosition;
}

std::vector<DirectX::XMVECTOR> UndoMove::getMovedObjectsOriginalPositions()
{
	return movedObjectsOriginalPositions;
}
