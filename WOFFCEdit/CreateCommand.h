#pragma once
#include "Commands.h"
#include <iostream>
class CreateCommand: public Commands
{
public:
	CreateCommand();
	void performAction(std::vector<DisplayObject>&objects);
	Commands::CommandType getType();
	void setType(Commands::CommandType);
};

