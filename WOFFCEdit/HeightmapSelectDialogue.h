#pragma once
#include "resource.h"
#include "afxwin.h"
#include "ToolMain.h"
#include <string>

class HeightmapSelectDialogue: public CDialogEx
{
	DECLARE_DYNAMIC(HeightmapSelectDialogue)

public:
	HeightmapSelectDialogue(CWnd* pParent = NULL);
	virtual ~HeightmapSelectDialogue();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG7 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void End();		//kill the dialogue
	afx_msg void Cancel();
	ToolMain* m_ToolSystem;	//Instance of Tool System that we interface to. 

	std::string newPath;

	DECLARE_MESSAGE_MAP()
public:
	// Control variable for more efficient access of the listbox
	//CMFCEditBrowseCtrl m_wndFileEdit;
	virtual BOOL OnInitDialog() override;
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedConvert();
	void setToolSystem(ToolMain* toolsystem);
};

