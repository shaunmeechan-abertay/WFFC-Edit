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
	bool multipick;
	bool copy;
	bool paste;
};
