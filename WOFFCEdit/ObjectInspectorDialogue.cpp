#include "stdafx.h"
#include "ObjectInspectorDialogue.h"

IMPLEMENT_DYNAMIC(ObjectInspectorDialogue, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(ObjectInspectorDialogue, CDialogEx)
	ON_COMMAND(IDOK, &ObjectInspectorDialogue::End)					//ok button
	ON_BN_CLICKED(IDC_BUTTON1, &ObjectInspectorDialogue::OnBnClickedLoadTexture)
	ON_BN_CLICKED(IDC_BUTTON2, &ObjectInspectorDialogue::OnBnClickedLoadModel)
END_MESSAGE_MAP()


ObjectInspectorDialogue::ObjectInspectorDialogue(CWnd* pParent)
{
}

ObjectInspectorDialogue::~ObjectInspectorDialogue()
{
}

void ObjectInspectorDialogue::DoDataExchange(CDataExchange* pDX)
{
	//Credit: https://stackoverflow.com/questions/10477786/how-to-get-numeric-value-from-edit-control
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1_XPos, XPos);
	DDX_Text(pDX, IDC_EDIT1_YPos, YPos);
	DDX_Text(pDX, IDC_EDIT1_ZPos, ZPos);
	DDX_Text(pDX, IDC_EDIT1_XScale, XScale);
	DDX_Text(pDX, IDC_EDIT1_YScale, YScale);
	DDX_Text(pDX, IDC_EDIT1_ZScale, ZScale);
	DDX_Text(pDX, IDC_EDIT1_XRotation, XRot);
	DDX_Text(pDX, IDC_EDIT1_YRotation, YRot);
	DDX_Text(pDX, IDC_EDIT1_ZRotation, ZRot);
}

void ObjectInspectorDialogue::End()
{
	UpdateData(true);
	DirectX::XMVECTOR position = DirectX::XMVectorSet(XPos, YPos, ZPos, 0);
	DirectX::XMVECTOR scale = DirectX::XMVectorSet(XScale, YScale, ZScale, 0);
	DirectX::XMVECTOR rotation = DirectX::XMVectorSet(XRot, YRot, ZRot, 0);
	//Here is where we get the selected object and work on it
	DisplayObject* selectedObject = m_ToolSystem->getSelectedGameObject();
	if (selectedObject == NULL)
	{
		DestroyWindow();
		return;
	}

	//set position
	selectedObject->m_position.x = position.m128_f32[0];
	selectedObject->m_position.y = position.m128_f32[1];
	selectedObject->m_position.z = position.m128_f32[2];

	//setorientation
	selectedObject->m_orientation.x = rotation.m128_f32[0];
	selectedObject->m_orientation.y = rotation.m128_f32[1];
	selectedObject->m_orientation.z = rotation.m128_f32[2];

	//set scale
	selectedObject->m_scale.x = scale.m128_f32[0];
	selectedObject->m_scale.y = scale.m128_f32[1];
	selectedObject->m_scale.z = scale.m128_f32[2];

	//Check model and texture now
	if (textureFile != "")
	{
		//There is a texture update
		//Load Texture
		HRESULT rs;
		CT2A convertedCString(textureFile);
		std::string convertedString(convertedCString);
		std::wstring texturewstr = StringToWCHART(convertedString.c_str());								//convect string to Wchar
		//This doesn't seem to work, I'm guessing pointer issue
		rs = DirectX::CreateDDSTextureFromFile(m_ToolSystem->getD3DDevice()->GetD3DDevice(), texturewstr.c_str(), nullptr, &selectedObject->m_texture_diffuse);	//load tex into Shader resource

		//if texture fails.  load error default
		if (rs)
		{
			DirectX::CreateDDSTextureFromFile(m_ToolSystem->getD3DDevice()->GetD3DDevice(), L"database/data/Error.dds", nullptr, &selectedObject->m_texture_diffuse);	//load tex into Shader resource
		}

	}

	m_ToolSystem->updateAllDragArrows();

	DestroyWindow();
}

BOOL ObjectInspectorDialogue::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return TRUE;
}

void ObjectInspectorDialogue::PostNcDestroy()
{
}

void ObjectInspectorDialogue::setToolSystem(ToolMain* toolsystem)
{
	m_ToolSystem = toolsystem;
}

void ObjectInspectorDialogue::OnBnClickedLoadTexture()
{
	//Credit: https://www.programmerall.com/article/50391274688/ 
	CFileDialog dlg(TRUE, _T("*.dds"), NULL, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("Image Files(*.dds)|*.dds|"), NULL);
	dlg.m_ofn.lpstrTitle = _T("Select Image");
	CString filename;
	if (dlg.DoModal() == IDOK)
	{
		POSITION fileNamesPosition = dlg.GetStartPosition();
		filename = dlg.GetNextPathName(fileNamesPosition);
		textureFile = filename;
	}

}

void ObjectInspectorDialogue::OnBnClickedLoadModel()
{
	//Credit: https://www.programmerall.com/article/50391274688/ 
	CFileDialog dlg(TRUE, _T("*.cmo"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("Model(*.cmo)|*.cmo|"), NULL);
	dlg.m_ofn.lpstrTitle = _T("Select Model");
	CString filename;
	if (dlg.DoModal() == IDOK)
	{
		POSITION fileNamesPosition = dlg.GetStartPosition();
		filename = dlg.GetNextPathName(fileNamesPosition);
		modelFile = filename;
	}
}
