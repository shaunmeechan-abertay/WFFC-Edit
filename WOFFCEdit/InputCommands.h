#pragma once

struct InputCommands
{
	bool forward;
	bool back;
	bool right;
	bool left;
	bool rotRight;
	bool rotLeft;
	bool rotUp;
	bool rotDown;
	bool deleteObject;
	bool UndoCommand;
	bool RedoCommand;
	int mouse_X;
	int mouse_Y;
	bool mouse_LB_Down;
	bool mouse_LB_Up;
	bool mouse_RB_Down;
	bool mouse_RB_Up;
	bool multipick;
	bool copy;
	bool paste;
	bool focusOnObject;
	bool CTRL;
};
