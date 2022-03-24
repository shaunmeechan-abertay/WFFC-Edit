#pragma once
#include "Commands.h"
#include "SceneObject.h"
#include "Model.h"
#include "DeleteCommand.h"
class CreateCommand: public Commands
{
public:
	CreateCommand();
	void performAction(std::vector<DisplayObject>&objects, DisplayObject deletedObject, std::unique_ptr<DirectX::EffectFactory> &m_fxFactory, bool isPaste = false, bool isUndo = false);
	void performAction(std::vector<DisplayObject>&objects, std::vector<DisplayObject>& objectsToCreate, std::unique_ptr<DirectX::EffectFactory> &m_fxFactory, bool isPaste = false, bool isUndo = false);
	Commands::CommandType getType();
	DisplayObject getCreatedObject();
	std::vector<DisplayObject> getCreatedObjects();
private:
	std::wstring StringToWCHART(std::string s);
	DisplayObject createdObject;
	std::vector<DisplayObject> createdObjects;
};

