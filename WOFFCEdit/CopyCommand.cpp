#include "CopyCommand.h"

CopyCommand::CopyCommand()
{
	type = Commands::CommandType::Copy;
}

void CopyCommand::performAction(std::vector<DisplayObject>& objects)
{
	for (int i = 0; i < objects.size(); i++)
	{
		DisplayObject newObject;

		//load model
		newObject.m_model = objects[i].m_model;	//get DXSDK to load model "False" for LH coordinate system (maya)

		//Load Texture
		newObject.m_texture_diffuse = objects[i].m_texture_diffuse;

		//apply new texture to models effect
		newObject.m_model->UpdateEffects([&](DirectX::IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
			{
				auto lights = dynamic_cast<DirectX::BasicEffect*>(effect);
				if (lights)
				{
					lights->SetTexture(newObject.m_texture_diffuse);
				}
			});

		//set position
		newObject.m_position.x = objects[i].m_position.x;
		newObject.m_position.y = objects[i].m_position.y + 5;
		newObject.m_position.z = objects[i].m_position.z;

		//setorientation
		newObject.m_orientation.x = objects[i].m_orientation.x;
		newObject.m_orientation.y = objects[i].m_orientation.y;
		newObject.m_orientation.z = objects[i].m_orientation.z;

		//set scale
		newObject.m_scale.x = objects[i].m_scale.x;
		newObject.m_scale.y = objects[i].m_scale.y;
		newObject.m_scale.z = objects[i].m_scale.z;

		//set wireframe / render flags
		newObject.m_render = objects[i].m_render;
		newObject.m_wireframe = false;

		newObject.m_light_type = objects[i].m_light_type;
		newObject.m_light_diffuse_r = objects[i].m_light_diffuse_r;
		newObject.m_light_diffuse_g = objects[i].m_light_diffuse_g;
		newObject.m_light_diffuse_b = objects[i].m_light_diffuse_b;
		newObject.m_light_specular_r = objects[i].m_light_specular_r;
		newObject.m_light_specular_g = objects[i].m_light_specular_g;
		newObject.m_light_specular_b = objects[i].m_light_specular_b;
		newObject.m_light_spot_cutoff = objects[i].m_light_spot_cutoff;
		newObject.m_light_constant = objects[i].m_light_constant;
		newObject.m_light_linear = objects[i].m_light_linear;
		newObject.m_light_quadratic = objects[i].m_light_quadratic;
		//Assign ID
		//BUG: This copies the ID, means ID isn't unique so deletion doesn't work correctly
		//Solution? Loop through objects to get higest ID then plus 1?
		newObject.m_ID = objects[i].m_ID;
		
		copiedObjects.push_back(newObject);
	}
}

void CopyCommand::performAction(DisplayObject* object)
{
	//load model
	copiedObject.m_model = object->m_model;	//get DXSDK to load model "False" for LH coordinate system (maya)

	//Load Texture
	copiedObject.m_texture_diffuse = object->m_texture_diffuse;

	//apply new texture to models effect
	copiedObject.m_model->UpdateEffects([&](DirectX::IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
		{
			auto lights = dynamic_cast<DirectX::BasicEffect*>(effect);
			if (lights)
			{
				lights->SetTexture(copiedObject.m_texture_diffuse);
			}
		});

	//set position
	copiedObject.m_position.x = object->m_position.x;
	copiedObject.m_position.y = object->m_position.y + 5;
	copiedObject.m_position.z = object->m_position.z;

	//setorientation
	copiedObject.m_orientation.x = object->m_orientation.x;
	copiedObject.m_orientation.y = object->m_orientation.y;
	copiedObject.m_orientation.z = object->m_orientation.z;

	//set scale
	copiedObject.m_scale.x = object->m_scale.x;
	copiedObject.m_scale.y = object->m_scale.y;
	copiedObject.m_scale.z = object->m_scale.z;

	//set wireframe / render flags
	copiedObject.m_render = object->m_render;
	copiedObject.m_wireframe = object->m_wireframe;

	copiedObject.m_light_type = object->m_light_type;
	copiedObject.m_light_diffuse_r = object->m_light_diffuse_r;
	copiedObject.m_light_diffuse_g = object->m_light_diffuse_g;
	copiedObject.m_light_diffuse_b = object->m_light_diffuse_b;
	copiedObject.m_light_specular_r = object->m_light_specular_r;
	copiedObject.m_light_specular_g = object->m_light_specular_g;
	copiedObject.m_light_specular_b = object->m_light_specular_b;
	copiedObject.m_light_spot_cutoff = object->m_light_spot_cutoff;
	copiedObject.m_light_constant = object->m_light_constant;
	copiedObject.m_light_linear = object->m_light_linear;
	copiedObject.m_light_quadratic = object->m_light_quadratic;
	//Assign ID
	copiedObject.m_ID = object->m_ID;
}

Commands::CommandType CopyCommand::getType()
{
	return type;
}

void CopyCommand::setType(Commands::CommandType newType)
{
	type = newType;
}

DisplayObject CopyCommand::getCopiedObject()
{
	return copiedObject;
}

std::vector<DisplayObject> CopyCommand::getCopiedObjects()
{
	return copiedObjects;
}