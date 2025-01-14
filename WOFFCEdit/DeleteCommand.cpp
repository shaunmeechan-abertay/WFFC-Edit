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
			deletedObject = objects.at(i);
			deletedObject.m_wireframe = false;
			objects.erase(objects.begin() + i);
			return;
		}
	}
}

void DeleteCommand::performAction(std::vector<DisplayObject>&objects, std::vector<DisplayObject*> ObjectsToDelete)
{
	//This is broken...
	//This is because the elements in selected object are stored as pointers
	//So when data is deleted from the display list the selcted object adapts
	//So what we need to do is take out the objects in ObjectToDelete (really only the IDs)
	std::vector<int> IDs;
	for (unsigned int i = 0; i < ObjectsToDelete.size(); i++)
	{
		IDs.push_back(ObjectsToDelete.at(i)->m_ID);
	}

	for (unsigned int  i = 0; i < (unsigned)IDs.size(); i++)
	{
		for (unsigned int j = 0; j < (unsigned)objects.size(); j++)
		{
			if (objects.at(j).m_ID == IDs.at(i))
			{
				objects.at(j).m_wireframe = false;
				deletedObjects.push_back(objects.at(j));
				objects.erase(objects.begin() + j);
				//objects.at(j).m_position.y = 10;
				break;
			}
		}
	}

}

Commands::CommandType DeleteCommand::getType()
{
	return type;
}

DisplayObject DeleteCommand::getDeletedObject()
{
	return deletedObject;
}

std::vector<DisplayObject> DeleteCommand::getDeletedObjects()
{
	return deletedObjects;
}
