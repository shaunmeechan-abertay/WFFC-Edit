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

	for (unsigned int i = 0; i < (unsigned)objects.size(); i++)
	{
		if (objects[i].m_ID == ID)
		{
			objects.erase(objects.begin() + i);
			return;
		}
	}
}

void DeleteCommand::performAction(std::vector<DisplayObject>&objects, std::vector<int>& IDs)
{

	for (unsigned int  i = 0; i < (unsigned)IDs.size(); i++)
	{
		for (unsigned int j = 0; j < (unsigned)objects.size(); j++)
		{
			if (objects[j].m_ID == IDs.at(i))
			{
				objects.erase(objects.begin() + j);
				break;
			}
		}
	}

}

Commands::CommandType DeleteCommand::getType()
{
	return type;
}
