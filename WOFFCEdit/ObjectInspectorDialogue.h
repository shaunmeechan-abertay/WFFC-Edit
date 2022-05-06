#pragma once
#include "resource.h"
#include "afxwin.h"
#include "ToolMain.h"
#include "Game.h"
#include "UndoManipulationCommand.h"
#include <stdlib.h>
#include <vector>
#include <string>
#include <filesystem>


class ObjectInspectorDialogue : public CDialogEx
{
	DECLARE_DYNAMIC(ObjectInspectorDialogue);
public:
	ObjectInspectorDialogue(CWnd* pParent = NULL);
	virtual ~ObjectInspectorDialogue();

	//Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG5 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void End();		//kill the dialogue

	CString textureFile;
	CString modelFile;
	ToolMain* m_ToolSystem;	//Instance of Tool System that we interface to. 
	Game* gameSystem;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	virtual void PostNcDestroy();
	void setToolSystem(ToolMain* toolsystem);
	afx_msg void OnBnClickedLoadTexture();
	afx_msg void OnBnClickedLoadModel();

	float XPos;
	float YPos;
	float ZPos;
	float XScale;
	float YScale;
	float ZScale;
	float XRot;
	float YRot;
	float ZRot;

};

