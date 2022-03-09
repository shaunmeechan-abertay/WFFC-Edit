#pragma once
#include "Commands.h"
#include <vector>

class DeleteCommand: public Commands
{
public:
	DeleteCommand();
	void performAction(std::vector<DisplayObject>&objects, std::vector<int>& IDs);
	void performAction(std::vector<DisplayObject>&objects, int ID);
	Commands::CommandType getType();
private:
	//ToolMain m_ToolSystem;	//Instance of Tool System that we interface to. 
};

