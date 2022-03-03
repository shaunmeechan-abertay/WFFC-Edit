#pragma once
#include "Commands.h"
class DeleteCommand: protected Commands
{
private:
	CommandType type;
public:
	DeleteCommand();
	void performAction(DisplayObject);
	Commands::CommandType getType();
};

