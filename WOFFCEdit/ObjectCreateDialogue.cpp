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
}

void ObjectCreateDialogue::End()
{
	m_ToolSystem->CreateNewGameObject(textureFile,modelFile);
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
//Issue, this only deals with jpg but texture could be any image, should at least handle png
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
//Issue, this only deals with jpg but texture could be any image, should at least handle png
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
