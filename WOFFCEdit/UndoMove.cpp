#include "UndoMove.h"

UndoMove::UndoMove()
{
	type = Commands::CommandType::UndoMove;
	movedObject = NULL;
}

void UndoMove::setup(DisplayObject* object, DirectX::XMVECTOR originalPosition)
{
	movedObject = object;
	movedObjectsOriginalPosition = originalPosition;
}

void UndoMove::setup(std::vector<DisplayObject*> objects, std::vector<DirectX::XMVECTOR> originalPositions)
{
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		movedObjects.push_back(objects[i]);
		movedObjectsOriginalPositions.push_back(originalPositions[i]);
	}
}

void UndoMove::performAction()
{
	if (movedObjects.empty() == true)
	{
		movedObjectsPreviousPosition = movedObject->m_position;
		movedObject->m_position = movedObjectsOriginalPosition;
	}
	else
	{
		for (unsigned int i = 0; i < movedObjects.size(); i++)
		{
			movedObjectsPreviousPositions.push_back(movedObjects[i]->m_position);
			movedObjects[i]->m_position = movedObjectsOriginalPositions[i];
		}
	}
}

DisplayObject* UndoMove::getMovedObject()
{
	return movedObject;
}

std::vector<DisplayObject*> UndoMove::getMovedObjects()
{
	return movedObjects;
}

DirectX::XMVECTOR UndoMove::getMovedObjectsPreviousPosition()
{
	return movedObjectsPreviousPosition;
}

std::vector<DirectX::XMVECTOR> UndoMove::getMovedObjectsPreviousPositions()
{
	return movedObjectsPreviousPositions;
}
