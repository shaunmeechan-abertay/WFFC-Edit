#pragma once
#include "Commands.h"
#include "DeviceResources.h"
#include "SceneObject.h"
#include "Model.h"
//Can remove this once it is working
#include <iostream>
class CreateCommand: public Commands
{
public:
	CreateCommand();
	void performAction(std::vector<DisplayObject>&objects, std::shared_ptr<DX::DeviceResources>& m_deviceResources, std::vector<SceneObject>&SceneGraph, std::unique_ptr<DirectX::EffectFactory> &m_fxFactory);
	Commands::CommandType getType();
	void setType(Commands::CommandType);
private:
	std::wstring StringToWCHART(std::string s);
};

