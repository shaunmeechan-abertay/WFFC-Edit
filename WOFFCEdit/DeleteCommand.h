#pragma once
#include "Commands.h"
class DeleteCommand: protected Commands
{
private:
	CommandType type;
public:
	DeleteCommand();
	void performAction(std::vector<DisplayObject>&);
	Commands::CommandType getType();
};

