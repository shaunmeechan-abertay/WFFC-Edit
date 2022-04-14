#pragma once
#include "Commands.h"
#include "SceneObject.h"
#include "Model.h"
#include "DeleteCommand.h"
#include "DeviceResources.h"
#include <filesystem>
#include <iostream>

class CreateCommand: public Commands
{
public:
	CreateCommand();
	//This is a copy action that recreates a deleted object
	void performAction(std::vector<DisplayObject>&objects, DisplayObject deletedObject, std::unique_ptr<DirectX::EffectFactory> &m_fxFactory, bool isPaste = false, bool isUndo = false);
	//This is a copy action that recreates deleted objects
	void performAction(std::vector<DisplayObject>&objects, std::vector<DisplayObject>& objectsToCreate, std::unique_ptr<DirectX::EffectFactory> &m_fxFactory, bool isPaste = false, bool isUndo = false);
	//This will make a brand new object using just the display list, it will create an object at a specified ID, if one is not provided it uses the first one in the list
	void performAction(std::vector<DisplayObject>&objects, int ID = 0);
	//This will make a brand new object using a passed in texture file and model
	void performAction(std::vector<DisplayObject>& objects, std::string textureFile, std::string modelFile, std::shared_ptr<DX::DeviceResources>,DirectX::IEffectFactory& m_fxFactory);
	Commands::CommandType getType();
	DisplayObject getCreatedObject();
	std::vector<DisplayObject*> getCreatedObjects();
private:
	std::wstring StringToWCHART(std::string s);
	DisplayObject createdObject;
	std::vector<DisplayObject*> createdObjects;
};

