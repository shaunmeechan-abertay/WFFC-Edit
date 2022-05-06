#include "RedoManipulationCommand.h"

RedoManipulationCommand::RedoManipulationCommand()
{
	type = Commands::CommandType::RedoManipulation;
}

void RedoManipulationCommand::setup(DisplayObject* object)
{
	storedObject = new DisplayObject;
	//load model
	storedObject->m_model = object->m_model;	//get DXSDK to load model "False" for LH coordinate system (maya)

	//Load Texture
	storedObject->m_texture_diffuse = object->m_texture_diffuse;

	//apply new texture to models effect
	storedObject->m_model->UpdateEffects([&](DirectX::IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
		{
			auto lights = dynamic_cast<DirectX::BasicEffect*>(effect);
			if (lights)
			{
				lights->SetTexture(storedObject->m_texture_diffuse);
			}
		});

	//set model and texture paths
	storedObject->m_texturePath = object->m_texturePath;
	storedObject->m_modelPath = object->m_modelPath;


	//set position
	storedObject->m_position.x = object->m_position.x;
	storedObject->m_position.y = object->m_position.y;
	storedObject->m_position.z = object->m_position.z;

	//setorientation
	storedObject->m_orientation.x = object->m_orientation.x;
	storedObject->m_orientation.y = object->m_orientation.y;
	storedObject->m_orientation.z = object->m_orientation.z;

	//set scale
	storedObject->m_scale.x = object->m_scale.x;
	storedObject->m_scale.y = object->m_scale.y;
	storedObject->m_scale.z = object->m_scale.z;

	//set wireframe / render flags
	storedObject->m_render = object->m_render;
	storedObject->m_wireframe = object->m_wireframe;

	storedObject->m_light_type = object->m_light_type;
	storedObject->m_light_diffuse_r = object->m_light_diffuse_r;
	storedObject->m_light_diffuse_g = object->m_light_diffuse_g;
	storedObject->m_light_diffuse_b = object->m_light_diffuse_b;
	storedObject->m_light_specular_r = object->m_light_specular_r;
	storedObject->m_light_specular_g = object->m_light_specular_g;
	storedObject->m_light_specular_b = object->m_light_specular_b;
	storedObject->m_light_spot_cutoff = object->m_light_spot_cutoff;
	storedObject->m_light_constant = object->m_light_constant;
	storedObject->m_light_linear = object->m_light_linear;
	storedObject->m_light_quadratic = object->m_light_quadratic;
	//Assign ID
	storedObject->m_ID = object->m_ID;
}

void RedoManipulationCommand::setup(std::vector<DisplayObject*> objects)
{
	for (int i = 0; i < objects.size(); i++)
	{
		DisplayObject newObject;

		//load model
		newObject.m_model = objects.at(i)->m_model;	//get DXSDK to load model "False" for LH coordinate system (maya)

		//Load Texture
		newObject.m_texture_diffuse = objects.at(i)->m_texture_diffuse;

		//apply new texture to models effect
		newObject.m_model->UpdateEffects([&](DirectX::IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
			{
				auto lights = dynamic_cast<DirectX::BasicEffect*>(effect);
				if (lights)
				{
					lights->SetTexture(newObject.m_texture_diffuse);
				}
			});

		//Set texture and model paths
		newObject.m_texturePath = objects.at(i)->m_texturePath;
		newObject.m_modelPath = objects.at(i)->m_modelPath;

		//set position
		newObject.m_position.x = objects.at(i)->m_position.x;
		newObject.m_position.y = objects.at(i)->m_position.y + 5;
		newObject.m_position.z = objects.at(i)->m_position.z;

		//setorientation
		newObject.m_orientation.x = objects.at(i)->m_orientation.x;
		newObject.m_orientation.y = objects.at(i)->m_orientation.y;
		newObject.m_orientation.z = objects.at(i)->m_orientation.z;

		//set scale
		newObject.m_scale.x = objects.at(i)->m_scale.x;
		newObject.m_scale.y = objects.at(i)->m_scale.y;
		newObject.m_scale.z = objects.at(i)->m_scale.z;

		//set wireframe / render flags
		newObject.m_render = objects.at(i)->m_render;
		newObject.m_wireframe = false;

		newObject.m_light_type = objects.at(i)->m_light_type;
		newObject.m_light_diffuse_r = objects.at(i)->m_light_diffuse_r;
		newObject.m_light_diffuse_g = objects.at(i)->m_light_diffuse_g;
		newObject.m_light_diffuse_b = objects.at(i)->m_light_diffuse_b;
		newObject.m_light_specular_r = objects.at(i)->m_light_specular_r;
		newObject.m_light_specular_g = objects.at(i)->m_light_specular_g;
		newObject.m_light_specular_b = objects.at(i)->m_light_specular_b;
		newObject.m_light_spot_cutoff = objects.at(i)->m_light_spot_cutoff;
		newObject.m_light_constant = objects.at(i)->m_light_constant;
		newObject.m_light_linear = objects.at(i)->m_light_linear;
		newObject.m_light_quadratic = objects.at(i)->m_light_quadratic;
		//Assign ID
		newObject.m_ID = objects.at(i)->m_ID;

		storedObjects.push_back(&newObject);
	}
}

void RedoManipulationCommand::performAction(std::vector<DisplayObject>* displayList)
{
	if (storedObjects.empty() == true)
	{
		//Find the object
		for (unsigned int i = 0; i < displayList->size(); i++)
		{
			if (storedObject->m_ID == displayList->at(i).m_ID)
			{
				tempObject = displayList->at(i);
				displayList->at(i) = *storedObject;
				storedObject = &tempObject;
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < storedObjects.size(); i++)
		{
			for (unsigned int j = 0; j < displayList->size(); j++)
			{
				if (storedObjects[i]->m_ID == displayList->at(j).m_ID)
				{
					tempObject = displayList->at(i);
					displayList->at(j) = *storedObjects[i];
					storedObjects.at(i) = &tempObject;
					break;
				}
			}
		}
	}
}

DisplayObject* RedoManipulationCommand::getStoredObject()
{
	return storedObject;
}

std::vector<DisplayObject*> RedoManipulationCommand::getStoredObjects()
{
	return storedObjects;
}
