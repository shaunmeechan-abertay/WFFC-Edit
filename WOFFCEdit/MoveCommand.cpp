#include "MoveCommand.h"

MoveCommand::MoveCommand()
{
	type = Commands::CommandType::Move;
}

void MoveCommand::setup(int objectID, DirectX::XMVECTOR originalPosition)
{
	movedObjectID = objectID;
	movedObjectsOriginalPosition = originalPosition;
}

void MoveCommand::setup(std::vector<int> objectsIDs, std::vector<DirectX::XMVECTOR> originalPositons)
{
	for (unsigned int i = 0; i < objectsIDs.size(); i++)
	{
		movedObjectsIDs.push_back(objectsIDs[i]);
		movedObjectsOriginalPositions.push_back(originalPositons[i]);
	}
}

void MoveCommand::performAction(std::vector<DisplayObject>* displayList)
{
	if (movedObjectsIDs.empty() == true)
	{
		//Find the object
		for (unsigned int i = 0; i < displayList->size(); i++)
		{
			if (movedObjectID == displayList->at(i).m_ID)
			{
				movedObjectsPreviousPosition = displayList->at(i).m_position;
				displayList->at(i).m_position = movedObjectsOriginalPosition;
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < movedObjectsIDs.size(); i++)
		{
			for (unsigned int i = 0; i < displayList->size(); i++)
			{
				if (movedObjectsIDs[i] == displayList->at(i).m_ID)
				{
					movedObjectsPreviousPositions.push_back(displayList->at(i).m_position);
					displayList->at(i).m_position = movedObjectsOriginalPositions[i];
				}
			}
		}
	}
}

int MoveCommand::getMovedObjectsID()
{
	return movedObjectID;
}

std::vector<int> MoveCommand::getMovedObjectsIDs()
{
	return movedObjectsIDs;
}

DirectX::XMVECTOR MoveCommand::getMovedObjectsPreviousPosition()
{
	return movedObjectsPreviousPosition;
}

std::vector<DirectX::XMVECTOR> MoveCommand::getMovedObjectsPreviousPositions()
{
	return movedObjectsPreviousPositions;
}
