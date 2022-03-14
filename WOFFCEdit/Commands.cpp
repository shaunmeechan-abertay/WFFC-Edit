#include "Commands.h"

void Commands::performAction(DisplayObject)
{
	return;
}

DisplayObject Commands::getDeletedObject()
{
	return DisplayObject();
}

Commands::CommandType Commands::getType()
{
	return type;
}
