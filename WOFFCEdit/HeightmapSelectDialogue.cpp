#include "stdafx.h"
#include "HeightmapSelectDialogue.h"

// TextureSelectDialogue dialog
IMPLEMENT_DYNAMIC(HeightmapSelectDialogue, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(HeightmapSelectDialogue, CDialogEx)
	ON_COMMAND(IDCANCEL, &HeightmapSelectDialogue::Cancel)
	ON_COMMAND(IDOK, &HeightmapSelectDialogue::OnBnClickedConvert)
	ON_BN_CLICKED(IDC_BUTTON1, &HeightmapSelectDialogue::OnBnClickedOk)
END_MESSAGE_MAP()

HeightmapSelectDialogue::HeightmapSelectDialogue(CWnd* pParent)
{

}

HeightmapSelectDialogue::~HeightmapSelectDialogue()
{
}

void HeightmapSelectDialogue::DoDataExchange(CDataExchange* pDX)
{
}

void HeightmapSelectDialogue::End()
{
	DestroyWindow();
	return;
}

void HeightmapSelectDialogue::Cancel()
{
	DestroyWindow();
	return;
}

BOOL HeightmapSelectDialogue::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	newPath = "";
	return TRUE;
}

void HeightmapSelectDialogue::PostNcDestroy()
{
}

void HeightmapSelectDialogue::OnBnClickedOk()
{
	//Credit: https://www.programmerall.com/article/50391274688/ 
//Issue, this only deals with jpg but texture could be any image, should at least handle png
	CFileDialog dlg(TRUE, _T("*.RAW"), NULL, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("RAW Files(*.RAW)|*.RAW|"), NULL);
	dlg.m_ofn.lpstrTitle = _T("Select Image");
	CString filename;
	if (dlg.DoModal() == IDOK)
	{
		POSITION fileNamesPosition = dlg.GetStartPosition();
		while (fileNamesPosition != NULL)
		{
			filename = dlg.GetNextPathName(fileNamesPosition);
			newPath = CT2CA(filename);
		}
	}
}

void HeightmapSelectDialogue::OnBnClickedConvert()
{
	if (newPath == "")
	{
		End();
		return;
	}
	m_ToolSystem->getGameSystem()->loadNewHeightmap(newPath);
	End();
	return;
}

void HeightmapSelectDialogue::setToolSystem(ToolMain* toolsystem)
{
	m_ToolSystem = toolsystem;
}
