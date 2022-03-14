#pragma once
#include "Commands.h"
#include "DeviceResources.h"
#include "SceneObject.h"
#include "Model.h"
#include "DeleteCommand.h"
class CreateCommand: public Commands
{
public:
	CreateCommand();
	void performAction(std::vector<DisplayObject>&objects, std::shared_ptr<DX::DeviceResources>& m_deviceResources, DisplayObject deletedObject, std::unique_ptr<DirectX::EffectFactory> &m_fxFactory);
	Commands::CommandType getType();
	void setType(Commands::CommandType);
private:
	std::wstring StringToWCHART(std::string s);
};

