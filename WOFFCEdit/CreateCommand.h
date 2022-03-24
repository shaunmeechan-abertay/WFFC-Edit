//TODO: Make a perform action that makes an object just using the display list (e.g. no need for an input object)
#pragma once
#include "Commands.h"
#include "SceneObject.h"
#include "Model.h"
#include "DeleteCommand.h"
class CreateCommand: public Commands
{
public:
	CreateCommand();
	//This is a copy action that recreates a deleted object
	void performAction(std::vector<DisplayObject>&objects, DisplayObject deletedObject, std::unique_ptr<DirectX::EffectFactory> &m_fxFactory, bool isPaste = false, bool isUndo = false);
	//This is a copy action that recreates deleted objects
	void performAction(std::vector<DisplayObject>&objects, std::vector<DisplayObject>& objectsToCreate, std::unique_ptr<DirectX::EffectFactory> &m_fxFactory, bool isPaste = false, bool isUndo = false);
	Commands::CommandType getType();
	DisplayObject getCreatedObject();
	std::vector<DisplayObject> getCreatedObjects();
private:
	std::wstring StringToWCHART(std::string s);
	DisplayObject createdObject;
	std::vector<DisplayObject> createdObjects;
};

