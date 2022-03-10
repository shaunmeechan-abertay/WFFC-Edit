#include "CreateCommand.h"

CreateCommand::CreateCommand()
{
	type = CommandType::Create;
}

void CreateCommand::performAction(std::vector<DisplayObject>& objects, std::shared_ptr<DX::DeviceResources> &m_deviceResources, std::vector<SceneObject> &SceneGraph, std::unique_ptr<DirectX::EffectFactory>& m_fxFactory)
{
	//objects.pushBack(object);
	std::cout << "Create called!";

	auto device = m_deviceResources->GetD3DDevice();
	auto devicecontext = m_deviceResources->GetD3DDeviceContext();

	//create a temp display object that we will populate then append to the display list.
	DisplayObject newDisplayObject;

	//load model
	std::wstring modelwstr = StringToWCHART(SceneGraph.at(0).model_path);							//convect string to Wchar
	newDisplayObject.m_model = DirectX::Model::CreateFromCMO(device, modelwstr.c_str(), *m_fxFactory, true);	//get DXSDK to load model "False" for LH coordinate system (maya)

	//Load Texture
	std::wstring texturewstr = StringToWCHART(SceneGraph.at(0).tex_diffuse_path);								//convect string to Wchar
	HRESULT rs;
	rs = DirectX::CreateDDSTextureFromFile(device, texturewstr.c_str(), nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource

	//if texture fails.  load error default
	if (rs)
	{
		DirectX::CreateDDSTextureFromFile(device, L"database/data/Error.dds", nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource
	}

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
	newDisplayObject.m_position.x = SceneGraph.at(0).posX;
	newDisplayObject.m_position.y = SceneGraph.at(0).posY;
	newDisplayObject.m_position.z = SceneGraph.at(0).posZ;

	//setorientation
	newDisplayObject.m_orientation.x = SceneGraph.at(0).rotX;
	newDisplayObject.m_orientation.y = SceneGraph.at(0).rotY;
	newDisplayObject.m_orientation.z = SceneGraph.at(0).rotZ;

	//set scale
	newDisplayObject.m_scale.x = SceneGraph.at(0).scaX;
	newDisplayObject.m_scale.y = SceneGraph.at(0).scaY;
	newDisplayObject.m_scale.z = SceneGraph.at(0).scaZ;

	//set wireframe / render flags
	newDisplayObject.m_render = SceneGraph.at(0).editor_render;
	newDisplayObject.m_wireframe = SceneGraph.at(0).editor_wireframe;

	newDisplayObject.m_light_type = SceneGraph.at(0).light_type;
	newDisplayObject.m_light_diffuse_r = SceneGraph.at(0).light_diffuse_r;
	newDisplayObject.m_light_diffuse_g = SceneGraph.at(0).light_diffuse_g;
	newDisplayObject.m_light_diffuse_b = SceneGraph.at(0).light_diffuse_b;
	newDisplayObject.m_light_specular_r = SceneGraph.at(0).light_specular_r;
	newDisplayObject.m_light_specular_g = SceneGraph.at(0).light_specular_g;
	newDisplayObject.m_light_specular_b = SceneGraph.at(0).light_specular_b;
	newDisplayObject.m_light_spot_cutoff = SceneGraph.at(0).light_spot_cutoff;
	newDisplayObject.m_light_constant = SceneGraph.at(0).light_constant;
	newDisplayObject.m_light_linear = SceneGraph.at(0).light_linear;
	newDisplayObject.m_light_quadratic = SceneGraph.at(0).light_quadratic;
	//Assign ID
	newDisplayObject.m_ID = SceneGraph.at(0).ID;

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
