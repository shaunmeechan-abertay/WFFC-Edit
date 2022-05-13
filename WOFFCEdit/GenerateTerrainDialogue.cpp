#include "stdafx.h"
#include "GenerateTerrainDialogue.h"

IMPLEMENT_DYNAMIC(GenerateTerrainDialogue, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(GenerateTerrainDialogue, CDialogEx)
	ON_COMMAND(IDOK, &GenerateTerrainDialogue::End)					//ok button
END_MESSAGE_MAP()

GenerateTerrainDialogue::GenerateTerrainDialogue(CWnd* pParent)
{
}

GenerateTerrainDialogue::~GenerateTerrainDialogue()
{
}

void GenerateTerrainDialogue::DoDataExchange(CDataExchange* pDX)
{
	//Credit: https://stackoverflow.com/questions/10477786/how-to-get-numeric-value-from-edit-control
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, maxHeight);
	DDX_Text(pDX, IDC_EDIT2, maxWidth);

}

void GenerateTerrainDialogue::End()
{
	gameSystem = m_ToolSystem->getGameSystem();
	UpdateData(true);
	gameSystem->generateNewTerrain(maxHeight, maxWidth);
	DestroyWindow();
}

BOOL GenerateTerrainDialogue::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return TRUE;
}

void GenerateTerrainDialogue::PostNcDestroy()
{
}

void GenerateTerrainDialogue::setToolSystem(ToolMain* toolSystem)
{
	m_ToolSystem = toolSystem;
}
