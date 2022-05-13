#pragma once
#include "resource.h"
#include "afxwin.h"
#include <vector>
#include <string>

class TextureSelectDialogue: public CDialogEx
{
	DECLARE_DYNAMIC(TextureSelectDialogue)

public:
	TextureSelectDialogue(CWnd* pParent = NULL);
	virtual ~TextureSelectDialogue();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void End();		//kill the dialogue
	afx_msg void Cancel();

	std::vector<CString>all_sel_files;
	DECLARE_MESSAGE_MAP()
public:
	// Control variable for more efficient access of the listbox
	//CMFCEditBrowseCtrl m_wndFileEdit;
	virtual BOOL OnInitDialog() override;
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedConvert();

};

INT_PTR CALLBACK SelectProc(HWND   hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam);
