//ISSUE: These use lot's of memory since there are six around each object.
//While if you are only selecting one that's not a major issue when you start multiselecting it get's bad
//We need to find a way to share the model and texture between all arrows
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

private:
	std::wstring StringToWCHART(std::string s);
};