#pragma once
#include "Commands.h"
class DeleteCommand: public Commands
{
public:
	DeleteCommand();
	void performAction(std::vector<DisplayObject>&);
	Commands::CommandType getType();
};

