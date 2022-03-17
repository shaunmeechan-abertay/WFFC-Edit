#pragma once
#include "Commands.h"
class CopyCommand : public Commands
{
	CopyCommand();
	void performAction(std::vector<DisplayObject>& objects);
	void performAction(DisplayObject& object);
	Commands::CommandType getType();
	//Does this need to exist?
	void setType(Commands::CommandType newType);

private:
	DisplayObject copiedObject;
	std::vector<DisplayObject> copiedObjects;
};

