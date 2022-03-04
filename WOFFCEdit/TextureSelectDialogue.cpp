// TextureSelectDialogue.cpp : implementation file

#include "stdafx.h"
#include "TextureSelectDialogue.h"

// TextureSelectDialogue dialog
IMPLEMENT_DYNAMIC(TextureSelectDialogue, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(TextureSelectDialogue, CDialogEx)
	ON_COMMAND(IDOK, &TextureSelectDialogue::End)					//ok button
	ON_BN_CLICKED(IDOK, &TextureSelectDialogue::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &TextureSelectDialogue::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON2, &TextureSelectDialogue::OnBnClickedConvert)
END_MESSAGE_MAP()

TextureSelectDialogue::TextureSelectDialogue(CWnd* pParent)
{
}

TextureSelectDialogue::~TextureSelectDialogue()
{
}

void TextureSelectDialogue::DoDataExchange(CDataExchange* pDX)
{
}

void TextureSelectDialogue::End()
{
	DestroyWindow();
}

BOOL TextureSelectDialogue::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return TRUE;
}

void TextureSelectDialogue::PostNcDestroy()
{
}

void TextureSelectDialogue::OnBnClickedOk()
{
	//Credit: https://www.programmerall.com/article/50391274688/ 
	//Isse, this only deal with jpg but texture could be any image, should at least handle png
	CFileDialog dlg(TRUE, _T("*.jpg"), NULL, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("Image Files(*.jpg)|*.jpg|"), NULL);
	dlg.m_ofn.lpstrTitle = _T("Select Image");
	CString filename;
	if (dlg.DoModal() == IDOK)
	{
		POSITION fileNamesPosition = dlg.GetStartPosition();
		while (fileNamesPosition != NULL)
		{
			filename = dlg.GetNextPathName(fileNamesPosition);
			all_sel_files.push_back(filename);
		}
	}
}

void TextureSelectDialogue::OnBnClickedConvert()
{
	system("cmd.exe /k texconv -r C:/Textures/*.png");
}
