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
				DirectX::XMVECTOR tempPosition = displayList->at(i).m_position;
				displayList->at(i).m_position = movedObjectsOriginalPosition;
				movedObjectsOriginalPosition = tempPosition;
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
					movedObjectsOriginalPositions.push_back(displayList->at(j).m_position);
					displayList->at(j).m_position = movedObjectsOriginalPositions[i];
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

DirectX::XMVECTOR MoveCommand::getMovedObjectsOriginalPosition()
{
	return movedObjectsOriginalPosition;
}

std::vector<DirectX::XMVECTOR> MoveCommand::getMovedObjectsOriginalPositions()
{
	return movedObjectsOriginalPositions;
}
