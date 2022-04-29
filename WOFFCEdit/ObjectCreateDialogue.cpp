#include "stdafx.h"
#include "ObjectCreateDialogue.h"

// TextureSelectDialogue dialog
IMPLEMENT_DYNAMIC(ObjectCreateDialogue, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(ObjectCreateDialogue, CDialogEx)
	ON_COMMAND(IDOK, &ObjectCreateDialogue::End)					//ok button
	ON_BN_CLICKED(IDC_BUTTON1, &ObjectCreateDialogue::OnBnClickedLoadTexture)
	ON_BN_CLICKED(IDC_BUTTON2, &ObjectCreateDialogue::OnBnClickedLoadModel)
END_MESSAGE_MAP()


ObjectCreateDialogue::ObjectCreateDialogue(CWnd* pParent)
{
}

ObjectCreateDialogue::~ObjectCreateDialogue()
{
}

void ObjectCreateDialogue::DoDataExchange(CDataExchange* pDX)
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

void ObjectCreateDialogue::End()
{
	UpdateData(true);
	DirectX::XMVECTOR position = DirectX::XMVectorSet(XPos, YPos, ZPos, 0);
	DirectX::XMVECTOR scale = DirectX::XMVectorSet(XScale, YScale, ZScale, 0);
	DirectX::XMVECTOR rotation = DirectX::XMVectorSet(XRot, YRot, ZRot, 0);
	m_ToolSystem->CreateNewGameObject(textureFile,modelFile, position,scale,rotation);
	DestroyWindow();
}

BOOL ObjectCreateDialogue::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return TRUE;
}

void ObjectCreateDialogue::PostNcDestroy()
{
}

void ObjectCreateDialogue::setToolSystem(ToolMain* toolsystem)
{
	m_ToolSystem = toolsystem;
}

void ObjectCreateDialogue::OnBnClickedLoadTexture()
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

void ObjectCreateDialogue::OnBnClickedLoadModel()
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
