#include "Commands.h"

void Commands::performAction(DisplayObject)
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

Commands::CommandType Commands::getType()
{
	return type;
}
