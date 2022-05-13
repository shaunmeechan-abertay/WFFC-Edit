#pragma once
#include "resource.h"
#include "afxwin.h"
#include "ToolMain.h"
#include "Game.h"

class GenerateTerrainDialogue : public CDialogEx
{
	DECLARE_DYNAMIC(GenerateTerrainDialogue);
public:
	GenerateTerrainDialogue(CWnd* pParent = NULL);
	virtual ~GenerateTerrainDialogue();

	//Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG6 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg void End();
	afx_msg void Cancel();

	ToolMain* m_ToolSystem;
	Game* gameSystem;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	virtual void PostNcDestroy();
	void setToolSystem(ToolMain* toolSystem);
	float maxHeight;
	float maxWidth;
};

