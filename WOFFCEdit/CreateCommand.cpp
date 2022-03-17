#include "CreateCommand.h"

CreateCommand::CreateCommand()
{
	type = Commands::CommandType::Create;
}

void CreateCommand::performAction(std::vector<DisplayObject>& objects, std::shared_ptr<DX::DeviceResources> &m_deviceResources, DisplayObject deletedObject, std::unique_ptr<DirectX::EffectFactory>& m_fxFactory)
{
	auto device = m_deviceResources->GetD3DDevice();
	auto devicecontext = m_deviceResources->GetD3DDeviceContext();

	//create a temp display object that we will populate then append to the display list.
	DisplayObject newDisplayObject;

	//load model
	newDisplayObject.m_model = deletedObject.m_model;	//get DXSDK to load model "False" for LH coordinate system (maya)

	//Load Texture
	newDisplayObject.m_texture_diffuse = deletedObject.m_texture_diffuse;

	//apply new texture to models effect
	newDisplayObject.m_model->UpdateEffects([&](DirectX::IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
		{
			auto lights = dynamic_cast<DirectX::BasicEffect*>(effect);
			if (lights)
			{
				lights->SetTexture(newDisplayObject.m_texture_diffuse);
			}
		});

	//set position
	newDisplayObject.m_position.x = deletedObject.m_position.x;
	newDisplayObject.m_position.y = deletedObject.m_position.y;
	newDisplayObject.m_position.z = deletedObject.m_position.z;

	//setorientation
	newDisplayObject.m_orientation.x = deletedObject.m_orientation.x;
	newDisplayObject.m_orientation.y = deletedObject.m_orientation.y;
	newDisplayObject.m_orientation.z = deletedObject.m_orientation.z;

	//set scale
	newDisplayObject.m_scale.x = deletedObject.m_scale.x;
	newDisplayObject.m_scale.y = deletedObject.m_scale.y;
	newDisplayObject.m_scale.z = deletedObject.m_scale.z;

	//set wireframe / render flags
	newDisplayObject.m_render = deletedObject.m_render;
	newDisplayObject.m_wireframe = false;

	newDisplayObject.m_light_type = deletedObject.m_light_type;
	newDisplayObject.m_light_diffuse_r = deletedObject.m_light_diffuse_r;
	newDisplayObject.m_light_diffuse_g = deletedObject.m_light_diffuse_g;
	newDisplayObject.m_light_diffuse_b = deletedObject.m_light_diffuse_b;
	newDisplayObject.m_light_specular_r = deletedObject.m_light_specular_r;
	newDisplayObject.m_light_specular_g = deletedObject.m_light_specular_g;
	newDisplayObject.m_light_specular_b = deletedObject.m_light_specular_b;
	newDisplayObject.m_light_spot_cutoff = deletedObject.m_light_spot_cutoff;
	newDisplayObject.m_light_constant = deletedObject.m_light_constant;
	newDisplayObject.m_light_linear = deletedObject.m_light_linear;
	newDisplayObject.m_light_quadratic = deletedObject.m_light_quadratic;
	//Assign ID
	newDisplayObject.m_ID = deletedObject.m_ID;

	objects.push_back(newDisplayObject);
}

Commands::CommandType CreateCommand::getType()
{
	return type;
}

void CreateCommand::setType(Commands::CommandType newType)
{
	type = newType;
}

std::wstring CreateCommand::StringToWCHART(std::string s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}
