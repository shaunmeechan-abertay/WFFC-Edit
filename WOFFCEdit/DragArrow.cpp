#include "DragArrow.h"


DragArrow::DragArrow()
{
	m_model = NULL;
	m_texture_diffuse = NULL;
	m_modelPath = "";
	m_texturePath = "";

	m_orientation.x = 0;
	m_orientation.y = 0;
	m_orientation.z = 0;
	m_position.x = 0;
	m_position.y = 0;
	m_position.z = 0;
	m_scale.x = 0.1f;
	m_scale.y = 0.1f;
	m_scale.z = 0.1f;
	m_render = true;
	m_wireframe = false;

	m_light_type = 0;
	m_light_diffuse_r = 0.0f;	m_light_diffuse_g = 0.0f;	m_light_diffuse_b = 0.0f;
	m_light_specular_r = 0.0f;	m_light_specular_g = 0.0f;	m_light_specular_b = 0.0f;
	m_light_spot_cutoff = 0.0f;
	m_light_constant = 0.0f;
	m_light_linear = 0.0f;
	m_light_quadratic = 0.0f;

	up = false;
	down = false;
	left = false;
	right = false;
	back = false;
	forward = false;
}

DragArrow::~DragArrow()
{
}

void DragArrow::cleanUp()
{
	//delete the texture & model
	m_model.reset();
	m_modelPath = "";
	m_texturePath = "";
	m_model.~shared_ptr();
	m_texture_diffuse->Release();
}

void DragArrow::setup(float xPos, float yPos, float zPos, float xRot, float yRot, float zRot, std::shared_ptr<DX::DeviceResources> device, DirectX::IEffectFactory& m_fxFactory, std::shared_ptr<DirectX::Model> model, ID3D11ShaderResourceView* texture)
{
	//Set texture and model path
	//Change these later
	m_texturePath = "database/data/Arrow5Albedo.dds";
	m_modelPath = "database/data/Arrow5.cmo";

		//load model
	if (model == NULL)
	{
		std::wstring modelwstr = StringToWCHART(m_modelPath);					//convect string to Wchar
		try
		{
			//This is very strange as it expects a dds with the same name as the model even if the dds isn't used
			m_model = DirectX::Model::CreateFromCMO(device->GetD3DDevice(), modelwstr.c_str(), m_fxFactory, true);	//get DXSDK to load model "False" for LH coordinate system (maya)
		}
		catch (const std::exception& ex)
		{
			//SOMETHING WENT WRONG
			std::printf(ex.what());
		}
	}
	else
	{
		m_model = model;
	}

	if (texture == NULL)
	{
		//Load Texture
		std::wstring texturewstr = StringToWCHART(m_texturePath);								//convect string to Wchar
		HRESULT rs;
		rs = DirectX::CreateDDSTextureFromFile(device->GetD3DDevice(), texturewstr.c_str(), nullptr, &m_texture_diffuse);	//load tex into Shader resource

		//if texture fails.  load error default
		if (rs)
		{
			DirectX::CreateDDSTextureFromFile(device->GetD3DDevice(), L"database/data/Error.dds", nullptr, &m_texture_diffuse);	//load tex into Shader resource
		}
	}
	else
	{
		m_texture_diffuse = texture;
	}

	//apply new texture to models effect
	m_model->UpdateEffects([&](DirectX::IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
		{
			auto lights = dynamic_cast<DirectX::BasicEffect*>(effect);
			if (lights)
			{
				lights->SetTexture(m_texture_diffuse);
			}
		});

	m_orientation.x = xRot;
	m_orientation.y = yRot;
	m_orientation.z = zRot;
	m_position.x = xPos;
	m_position.y = yPos;
	m_position.z = zPos;

}

void DragArrow::updateDragArrow()
{
	if (up == true)
	{
		m_position = DirectX::SimpleMath::Vector3(attachedObject->m_position.x, attachedObject->m_position.y + 1, attachedObject->m_position.z);
		return;
	}
	if (down == true)
	{
		m_position = DirectX::SimpleMath::Vector3(attachedObject->m_position.x, attachedObject->m_position.y - 1, attachedObject->m_position.z);
		return;
	}
	if (left == true)
	{
		m_position = DirectX::SimpleMath::Vector3(attachedObject->m_position.x - 1, attachedObject->m_position.y, attachedObject->m_position.z);
		return;
	}
	if (right == true)
	{
		m_position = DirectX::SimpleMath::Vector3(attachedObject->m_position.x + 1, attachedObject->m_position.y, attachedObject->m_position.z);
		return;
	}
	if (back == true)
	{
		m_position = DirectX::SimpleMath::Vector3(attachedObject->m_position.x, attachedObject->m_position.y, attachedObject->m_position.z - 1);
		return;
	}
	if (forward == true)
	{
		m_position = DirectX::SimpleMath::Vector3(attachedObject->m_position.x, attachedObject->m_position.y, attachedObject->m_position.z + 1);
		return;
	}
}

std::wstring DragArrow::StringToWCHART(std::string s)
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