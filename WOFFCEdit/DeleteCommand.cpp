#include "DeleteCommand.h"

DeleteCommand::DeleteCommand()
{
	type = CommandType::Delete;
}

void DeleteCommand::performAction(std::vector<DisplayObject> &objects, int ID)
{
	//This has to remove an object from m_displayList in Game.cpp
	//That is private
	//So we can take in the vector<DisplayObject> and the object to delete
	//And work on it that way
	for (int i = 0; i < objects.size(); i++)
	{
		objects.erase(objects.begin());
	}
}

Commands::CommandType DeleteCommand::getType()
{
	return type;
}
