#include "CopyCommand.h"

CopyCommand::CopyCommand()
{
	type == Commands::CommandType::Copy;
}

void CopyCommand::performAction(std::vector<DisplayObject>& objects)
{
	copiedObjects = objects;
}

void CopyCommand::performAction(DisplayObject& object)
{
	copiedObject = object;
}

Commands::CommandType CopyCommand::getType()
{
	return type;
}

void CopyCommand::setType(Commands::CommandType newType)
{
	type = newType;
}
