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
	//for (unsigned int i = 0; i < (unsigned)objects.size(); i++)
	//{
	//	objects.erase(objects.begin());
	//}

	for (unsigned int i = 0; i < (unsigned)objects.size(); i++)
	{
		if (objects[i].m_ID == ID)
		{
			objects.erase(objects.begin() + i);
			return;
		}
	}
}

Commands::CommandType DeleteCommand::getType()
{
	return type;
}
