#include "stdafx.h"
#include "ModelSelectDialogue.h"

// ModelSelectDialogue dialog
IMPLEMENT_DYNAMIC(ModelSelectDialogue, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(ModelSelectDialogue, CDialogEx)
	ON_COMMAND(IDOK, &ModelSelectDialogue::End)					//ok button
	ON_BN_CLICKED(IDCANCEL, &ModelSelectDialogue::End)
	ON_BN_CLICKED(IDC_BUTTON1, &ModelSelectDialogue::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON2, &ModelSelectDialogue::OnBnClickedConvert)
END_MESSAGE_MAP()

ModelSelectDialogue::ModelSelectDialogue(CWnd* pParent)
{
}

ModelSelectDialogue::~ModelSelectDialogue()
{
}

void ModelSelectDialogue::DoDataExchange(CDataExchange* pDX)
{
}

void ModelSelectDialogue::End()
{
	DestroyWindow();
}

BOOL ModelSelectDialogue::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return TRUE;
}

void ModelSelectDialogue::PostNcDestroy()
{
}

void ModelSelectDialogue::OnBnClickedOk()
{
	//Credit: https://www.programmerall.com/article/50391274688/ 
	CFileDialog dlg(TRUE, _T("*.obj"), NULL, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("Model Files(*._obj)|*._obj|"), NULL);
	dlg.m_ofn.lpstrTitle = _T("Select Models");
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

void ModelSelectDialogue::OnBnClickedConvert()
{
	std::string stringCommand = "cmd.exe /k meshconvert ";

	//Load all the paths into a string
	CString string;
	//Have to clear array this way as calling .clear at the end was casusing a crash
	int sizeOfArray = all_sel_files.size();
	for (int i = 0; i < sizeOfArray; i++)
	{
		if (i == 0)
		{
			string = all_sel_files.front().GetString();
			all_sel_files.erase(all_sel_files.begin());
		}
		else
		{
			string += " ";
			string += all_sel_files.front().GetString();
			all_sel_files.erase(all_sel_files.begin());
		}
	}
	//Annoying conversion stuff
	//Really a CString should just convert into a char* but it isn't
	//Possibly because of some compiler stuff
	//Copy the path into this since this works with MBCS (pressumably what the compiler is set to)
	string += " -cmo -flipz -y";
	//Convert the CString to a string and conncatenate
	CT2CA cStringToString(string);
	stringCommand += cStringToString;
	//Conver the string to a char* as that is what system needs
	const char* charCommand = stringCommand.c_str();
	system(charCommand);
	//system("cmd.exe /k meshconvert cup._obj -cmo -flipz -y");
}
