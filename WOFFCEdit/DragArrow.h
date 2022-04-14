#pragma once
#include "DisplayObject.h"
#include "DeviceResources.h"
#include "Model.h"

class DragArrow : public DisplayObject
{
public:
	DragArrow();
	~DragArrow();
	void cleanUp();
	void setup(float xPos, float yPos, float zPos, float xRot, float yRot, float zRot, std::shared_ptr<DX::DeviceResources> device, DirectX::IEffectFactory& m_fxFactory, std::shared_ptr<DirectX::Model> model = NULL, ID3D11ShaderResourceView* texture = NULL);
	bool up;
	bool down;
	bool left;
	bool right;
	bool back;
	bool forward;

	DisplayObject* attachedObject;
	void updateDragArrow();


private:
	std::wstring StringToWCHART(std::string s);
};