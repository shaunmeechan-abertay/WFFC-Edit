#include "Commands.h"

void Commands::performAction(DisplayObject)
{
	return;
}

void Commands::performAction(std::vector<DisplayObject>*)
{
	return;
}

DisplayObject Commands::getDeletedObject()
{
	return DisplayObject();
}

std::vector<DisplayObject> Commands::getDeletedObjects()
{
	return std::vector<DisplayObject>();
}

DisplayObject Commands::getCreatedObject()
{
	return DisplayObject();
}

std::vector<DisplayObject*> Commands::getCreatedObjects()
{
	return std::vector<DisplayObject*>();
}

int Commands::getMovedObjectsID()
{
	return 0;
}

std::vector<int> Commands::getMovedObjectsIDs()
{
	return std::vector<int>();
}

DirectX::XMVECTOR Commands::getMovedObjectsOriginalPosition()
{
	return DirectX::XMVECTOR();
}

std::vector<DirectX::XMVECTOR> Commands::getMovedObjectsOriginalPositions()
{
	return std::vector<DirectX::XMVECTOR>();
}

Commands::CommandType Commands::getType()
{
	return type;
}
