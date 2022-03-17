#pragma once
#include "Commands.h"
class CopyCommand : public Commands
{
public:
	CopyCommand();
	void performAction(std::vector<DisplayObject>& objects);
	void performAction(DisplayObject* object);
	Commands::CommandType getType();
	//Does this need to exist?
	void setType(Commands::CommandType newType);
	DisplayObject getCopiedObject();

private:
	//These will be copies of the input data
	//(we will make a new display objects(s) and fill it with the data sent in)
	//This is because the original could be deleted so we would just have a ref to bad data
	DisplayObject copiedObject;
	std::vector<DisplayObject> copiedObjects;
};

