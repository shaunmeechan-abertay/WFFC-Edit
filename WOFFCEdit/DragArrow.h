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
	void setup(float xPos, float yPos, float zPos, float xRot, float yRot, float zRot, std::shared_ptr<DX::DeviceResources> device, DirectX::IEffectFactory& m_fxFactory);
	bool up;
	bool down;
	bool left;
	bool right;
	bool back;
	bool forward;

private:
	std::wstring StringToWCHART(std::string s);
};