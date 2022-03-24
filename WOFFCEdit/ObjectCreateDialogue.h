#pragma once
#include "resource.h"
#include "afxwin.h"
#include "ToolMain.h"
#include <vector>
#include <string>

class ObjectCreateDialogue: public CDialogEx
{
	DECLARE_DYNAMIC(ObjectCreateDialogue);
public:
	ObjectCreateDialogue(CWnd* pParent = NULL);
	virtual ~ObjectCreateDialogue();

	//Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG4 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void End();		//kill the dialogue

	std::vector<CString>allTextureFiles;
	CString modelFile;
	ToolMain* m_ToolSystem;	//Instance of Tool System that we interface to. 
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	virtual void PostNcDestroy();
	void setToolSystem(ToolMain* toolsystem);
	afx_msg void OnBnClickedLoadTexture();
	afx_msg void OnBnClickedLoadModel();
	afx_msg void OnBnClickedCreated();
};

