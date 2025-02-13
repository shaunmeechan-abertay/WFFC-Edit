#include "CreateCommand.h"

CreateCommand::CreateCommand()
{
	type = Commands::CommandType::Create;
}

void CreateCommand::performAction(std::vector<DisplayObject>& objects, DisplayObject deletedObject, std::unique_ptr<DirectX::EffectFactory>& m_fxFactory, bool isPaste, bool isUndo)
{
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

	//Set texture and model path
	newDisplayObject.m_texturePath = deletedObject.m_texturePath;
	newDisplayObject.m_modelPath = deletedObject.m_modelPath;


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
	if (isPaste == true)
	{
		newDisplayObject.m_wireframe = true;
		newDisplayObject.m_selected = true;
	}
	else
	{
		newDisplayObject.m_wireframe = false;
		newDisplayObject.m_selected = false;
	}

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
	if (isUndo == true)
	{
		newDisplayObject.m_ID = deletedObject.m_ID;
	}
	else
	{
		unsigned int maxID = 0;
		for (unsigned int i = 0; i < objects.size(); i++)
		{
			if (objects[i].m_ID > maxID)
			{
				maxID = objects[i].m_ID;
			}
		}
		newDisplayObject.m_ID = maxID + 1;
	}

	objects.push_back(newDisplayObject);
	createdObject = newDisplayObject;
}

void CreateCommand::performAction(std::vector<DisplayObject>& objects, std::vector<DisplayObject>& objectsToCreate, std::unique_ptr<DirectX::EffectFactory>& m_fxFactory, bool isPaste, bool isUndo)
{
	unsigned int maxID = 0;
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i].m_ID > maxID)
		{
			maxID = objects[i].m_ID;
		}
	}

	for (int i = 0; i < objectsToCreate.size(); i++)
	{
		DisplayObject newObject;

		//load model
		newObject.m_model = objectsToCreate[i].m_model;	//get DXSDK to load model "False" for LH coordinate system (maya)

		//Load Texture
		newObject.m_texture_diffuse = objectsToCreate[i].m_texture_diffuse;

		//apply new texture to models effect
		newObject.m_model->UpdateEffects([&](DirectX::IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
			{
				auto lights = dynamic_cast<DirectX::BasicEffect*>(effect);
				if (lights)
				{
					lights->SetTexture(newObject.m_texture_diffuse);
				}
			});

		//Set texture and model path
		newObject.m_texturePath = objectsToCreate[i].m_texturePath;
		newObject.m_modelPath = objectsToCreate[i].m_modelPath;


		//set position
		newObject.m_position.x = objectsToCreate[i].m_position.x;
		newObject.m_position.y = objectsToCreate[i].m_position.y;
		newObject.m_position.z = objectsToCreate[i].m_position.z;

		//setorientation
		newObject.m_orientation.x = objectsToCreate[i].m_orientation.x;
		newObject.m_orientation.y = objectsToCreate[i].m_orientation.y;
		newObject.m_orientation.z = objectsToCreate[i].m_orientation.z;

		//set scale
		newObject.m_scale.x = objectsToCreate[i].m_scale.x;
		newObject.m_scale.y = objectsToCreate[i].m_scale.y;
		newObject.m_scale.z = objectsToCreate[i].m_scale.z;

		//set wireframe / render flags
		newObject.m_render = objectsToCreate[i].m_render;
		if (isPaste == true)
		{
			newObject.m_wireframe = true;
			newObject.m_selected = true;
		}
		else
		{
			newObject.m_wireframe = false;
			newObject.m_selected = false;
		}

		newObject.m_light_type = objectsToCreate[i].m_light_type;
		newObject.m_light_diffuse_r = objectsToCreate[i].m_light_diffuse_r;
		newObject.m_light_diffuse_g = objectsToCreate[i].m_light_diffuse_g;
		newObject.m_light_diffuse_b = objectsToCreate[i].m_light_diffuse_b;
		newObject.m_light_specular_r = objectsToCreate[i].m_light_specular_r;
		newObject.m_light_specular_g = objectsToCreate[i].m_light_specular_g;
		newObject.m_light_specular_b = objectsToCreate[i].m_light_specular_b;
		newObject.m_light_spot_cutoff = objectsToCreate[i].m_light_spot_cutoff;
		newObject.m_light_constant = objectsToCreate[i].m_light_constant;
		newObject.m_light_linear = objectsToCreate[i].m_light_linear;
		newObject.m_light_quadratic = objectsToCreate[i].m_light_quadratic;
		//Assign ID
		if (isUndo == true)
		{
			//If it is an undo we need to match the IDs
			newObject.m_ID = objectsToCreate[i].m_ID;
		}
		else
		{
			newObject.m_ID = maxID + 1;
			maxID++;
		}

		objects.push_back(newObject);
		createdObjects.push_back(newObject);
	}
}

void CreateCommand::performAction(std::vector<DisplayObject>& objects, int ID)
{
	//create a temp display object that we will populate then append to the display list.
	DisplayObject newDisplayObject;

	//load model
	newDisplayObject.m_model = objects[ID].m_model;	//get DXSDK to load model "False" for LH coordinate system (maya)

	//Load Texture
	newDisplayObject.m_texture_diffuse = objects[ID].m_texture_diffuse;

	//apply new texture to models effect
	newDisplayObject.m_model->UpdateEffects([&](DirectX::IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
		{
			auto lights = dynamic_cast<DirectX::BasicEffect*>(effect);
			if (lights)
			{
				lights->SetTexture(newDisplayObject.m_texture_diffuse);
			}
		});

	//Set texture and model path
	newDisplayObject.m_texturePath = objects[ID].m_texturePath;
	newDisplayObject.m_modelPath = objects[ID].m_modelPath;


	//set position
	newDisplayObject.m_position.x = objects[ID].m_position.x;
	newDisplayObject.m_position.y = objects[ID].m_position.y + 10;
	newDisplayObject.m_position.z = objects[ID].m_position.z;

	//setorientation
	newDisplayObject.m_orientation.x = objects[ID].m_orientation.x;
	newDisplayObject.m_orientation.y = objects[ID].m_orientation.y;
	newDisplayObject.m_orientation.z = objects[ID].m_orientation.z;

	//set scale
	newDisplayObject.m_scale.x = objects[ID].m_scale.x;
	newDisplayObject.m_scale.y = objects[ID].m_scale.y;
	newDisplayObject.m_scale.z = objects[ID].m_scale.z;

	//set wireframe / render flags
	newDisplayObject.m_render = objects[ID].m_render;
	newDisplayObject.m_wireframe = false;
	newDisplayObject.m_selected = false;
	newDisplayObject.m_light_type = objects[ID].m_light_type;
	newDisplayObject.m_light_diffuse_r = objects[ID].m_light_diffuse_r;
	newDisplayObject.m_light_diffuse_g = objects[ID].m_light_diffuse_g;
	newDisplayObject.m_light_diffuse_b = objects[ID].m_light_diffuse_b;
	newDisplayObject.m_light_specular_r = objects[ID].m_light_specular_r;
	newDisplayObject.m_light_specular_g = objects[ID].m_light_specular_g;
	newDisplayObject.m_light_specular_b = objects[ID].m_light_specular_b;
	newDisplayObject.m_light_spot_cutoff = objects[ID].m_light_spot_cutoff;
	newDisplayObject.m_light_constant = objects[ID].m_light_constant;
	newDisplayObject.m_light_linear = objects[ID].m_light_linear;
	newDisplayObject.m_light_quadratic = objects[ID].m_light_quadratic;
	//Assign ID
	unsigned int maxID = 0;
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		if (objects[i].m_ID > maxID)
		{
			maxID = objects[i].m_ID;
		}
	}
	newDisplayObject.m_ID = maxID + 1;
	objects.push_back(newDisplayObject);
	createdObject = newDisplayObject;
}

//Used to create a new object - called from MFC
void CreateCommand::performAction(std::vector<DisplayObject>& objects, std::string textureFile, std::string modelFile, DirectX::XMVECTOR position, DirectX::XMVECTOR scale, DirectX::XMVECTOR rotation, std::shared_ptr<DX::DeviceResources> device, DirectX::IEffectFactory& m_fxFactory)
{
	//create a temp display object that we will populate then append to the display list.
	DisplayObject newDisplayObject;

	//load model
	std::wstring modelwstr = StringToWCHART(modelFile);					//convect string to Wchar
	try
	{
		//This is very strange as it expects a dds with the same name as the model even if the dds isn't used
		newDisplayObject.m_model = DirectX::Model::CreateFromCMO(device->GetD3DDevice(), modelwstr.c_str(), m_fxFactory, true);	//get DXSDK to load model "False" for LH coordinate system (maya)
	}
	catch (const std::exception& ex)
	{
		//SOMETHING WENT WRONG
		std::printf(ex.what());
		MessageBox(NULL, (LPCWSTR)L"An error while loading the model", (LPCWSTR)L"Error", MB_OK | MB_ICONERROR);
		return;
	}

		//Load Texture
	std::wstring texturewstr = StringToWCHART(textureFile);								//convect string to Wchar
	HRESULT rs;
	rs = DirectX::CreateDDSTextureFromFile(device->GetD3DDevice(), texturewstr.c_str(), nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource
	
	//if texture fails.  load error default
	if (rs)
	{
		DirectX::CreateDDSTextureFromFile(device->GetD3DDevice(), L"database/data/Error.dds", nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource
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

	//Set texture and model path
	std::string filename = textureFile.substr(textureFile.rfind("\\"), textureFile.length() );
	std::filesystem::path relativePath = std::filesystem::relative(textureFile, "/database/data");

	newDisplayObject.m_texturePath = "database/data" + filename;

	filename = modelFile.substr(modelFile.rfind("\\"), modelFile.length());
	relativePath = std::filesystem::relative(modelFile, "/database/data");

	newDisplayObject.m_modelPath = "database/data" + filename;

	//set position
	newDisplayObject.m_position.x = position.m128_f32[0];
	newDisplayObject.m_position.y = position.m128_f32[1];
	newDisplayObject.m_position.z = position.m128_f32[2];

	//setorientation
	newDisplayObject.m_orientation.x = rotation.m128_f32[0];
	newDisplayObject.m_orientation.y = rotation.m128_f32[1];
	newDisplayObject.m_orientation.z = rotation.m128_f32[2];

	//set scale
	newDisplayObject.m_scale.x = scale.m128_f32[0];
	newDisplayObject.m_scale.y = scale.m128_f32[1];
	newDisplayObject.m_scale.z = scale.m128_f32[2];

	//set wireframe / render flags
	newDisplayObject.m_render = true;
	newDisplayObject.m_wireframe = false;
	newDisplayObject.m_selected = false;
	newDisplayObject.m_light_type = objects[0].m_light_type;
	newDisplayObject.m_light_diffuse_r = 1;
	newDisplayObject.m_light_diffuse_g = 1;
	newDisplayObject.m_light_diffuse_b = 1;
	newDisplayObject.m_light_specular_r = 1;
	newDisplayObject.m_light_specular_g = 1;
	newDisplayObject.m_light_specular_b = 1;
	newDisplayObject.m_light_spot_cutoff = objects[0].m_light_spot_cutoff;
	newDisplayObject.m_light_constant = objects[0].m_light_constant;
	newDisplayObject.m_light_linear = objects[0].m_light_linear;
	newDisplayObject.m_light_quadratic = objects[0].m_light_quadratic;
	//Assign ID
	unsigned int maxID = 0;
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		if (objects[i].m_ID > maxID)
		{
			maxID = objects[i].m_ID;
		}
	}
	newDisplayObject.m_ID = maxID + 1;
	objects.push_back(newDisplayObject);
	createdObject = newDisplayObject;
}

Commands::CommandType CreateCommand::getType()
{
	return type;
}

DisplayObject CreateCommand::getCreatedObject()
{
	return createdObject;
}

std::vector<DisplayObject> CreateCommand::getCreatedObjects()
{
	return createdObjects;
}

std::vector<DisplayObject*> CreateCommand::getCreatedObjectsAsPointers()
{
	std::vector<DisplayObject*> objectsAsPointers;
	for (unsigned int i = 0; i < createdObjects.size(); i++)
	{
		DisplayObject* obj = &createdObjects.at(i);
		objectsAsPointers.push_back(obj);
	}
	return objectsAsPointers;
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
