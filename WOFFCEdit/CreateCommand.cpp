#include "CreateCommand.h"

CreateCommand::CreateCommand()
{
	type = CommandType::Create;
}

void CreateCommand::performAction(std::vector<DisplayObject>& objects)
{
	//objects.pushBack(object);
	std::cout << "Create called!";

}

Commands::CommandType CreateCommand::getType()
{
	return type;
}

void CreateCommand::setType(Commands::CommandType newType)
{
	type = newType;
}
