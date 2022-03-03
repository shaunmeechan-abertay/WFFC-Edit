#pragma once
#include "Commands.h"

class DeleteCommand: public Commands
{
public:
	DeleteCommand();
	void performAction(std::vector<DisplayObject>&, int ID);
	Commands::CommandType getType();
private:
	//ToolMain m_ToolSystem;	//Instance of Tool System that we interface to. 
};

