//This is different to a create command. This just store a copy of the object and doesn't actually make it. This allows an paste command to use it
//Otherwise the code is pretty much idential to a create command (there is an argument to be made for merging the two but for readability and debugging might be best
//to keep them seperate)
#pragma once
#include "Commands.h"
class CopyCommand : public Commands
{
public:
	CopyCommand();
	//This is a copy action for a single object (used for undo and paste)
	void performAction(std::vector<DisplayObject>& objects);
	//This is a copy action for a multiple objects (used for undo and paste)
	void performAction(DisplayObject* object);
	Commands::CommandType getType();
	//Does this need to exist?
	void setType(Commands::CommandType newType);
	DisplayObject getCopiedObject();
	std::vector<DisplayObject> getCopiedObjects();

private:
	//These will be copies of the input data
	//(we will make a new display objects(s) and fill it with the data sent in)
	//This is because the original could be deleted so we would just have a ref to bad data
	DisplayObject copiedObject;
	std::vector<DisplayObject> copiedObjects;
};

