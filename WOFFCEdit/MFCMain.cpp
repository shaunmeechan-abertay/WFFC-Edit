#include "MFCMain.h"
//#include "resource.h"


BEGIN_MESSAGE_MAP(MFCMain, CWinApp)
	ON_COMMAND(ID_FILE_QUIT,	&MFCMain::MenuFileQuit)
	ON_COMMAND(ID_FILE_SAVETERRAIN, &MFCMain::MenuFileSaveTerrain)
	ON_COMMAND(ID_FILE_CONVERTTEXTURES, &MFCMain::MenuConvertTextures)
	ON_COMMAND(ID_FILE_CONVERTMODEL, &MFCMain::MenuConvertModel)
	ON_COMMAND(ID_EDIT_SELECT, &MFCMain::MenuEditSelect)
	ON_COMMAND(ID_EDIT_CREATENEWOBJECT, &MFCMain::MenuEditCreate)
	ON_COMMAND(ID_EDIT_OBJECTINSPECTOR, &MFCMain::MenuEditObjectInspector)
	ON_COMMAND(ID_BUTTON40001,	&MFCMain::ToolBarButton1)
	ON_COMMAND(ID_BUTTON40011,	&MFCMain::ToolBarSaveTerrain)
	ON_COMMAND(ID_EDIT_FLATTENTERRAIN,	&MFCMain::MenuEditFlattenTerrain)
	ON_COMMAND(ID_EDIT_GENERATENEWTERRAIN,	&MFCMain::MenuEditGenerateNewTerrain)
	ON_COMMAND(ID_EDIT_LOADNEWHEIGHTMAP,	&MFCMain::MenuEditLoadNewHeightmap)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TOOL, &CMyFrame::OnUpdatePage)
END_MESSAGE_MAP()

BOOL MFCMain::InitInstance()
{
	//instanciate the mfc frame
	m_frame = new CMyFrame();
	m_pMainWnd = m_frame;

	m_frame->Create(	NULL,
					_T("World Of Flim-Flam Craft Editor"),
					WS_OVERLAPPEDWINDOW,
					CRect(100, 100, 1024, 768),
					NULL,
					NULL,
					0,
					NULL
				);

	//show and set the window to run and update. 
	m_frame->ShowWindow(SW_SHOW);
	m_frame->UpdateWindow();


	//get the rect from the MFC window so we can get its dimensions
	m_toolHandle = m_frame->m_DirXView.GetSafeHwnd();				//handle of directX child window
	m_frame->m_DirXView.GetClientRect(&WindowRECT);
	m_width		= WindowRECT.Width();
	m_height	= WindowRECT.Height();

	m_ToolSystem.onActionInitialise(m_toolHandle, m_width, m_height);

	return TRUE;
}

int MFCMain::Run()
{
	MSG msg;
	BOOL bGotMsg;

	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		if (true)
		{
			bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
		}
		else
		{
			bGotMsg = (GetMessage(&msg, NULL, 0U, 0U) != 0);
		}

		if (bGotMsg)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			m_ToolSystem.UpdateInput(&msg);
		}
		else
		{	
			int ID = m_ToolSystem.getCurrentSelectionID();
			std::wstring statusString = L"Selected Object: " + std::to_wstring(ID);
			m_ToolSystem.Tick(&msg);
			m_GameSystem.setID(ID);

			//send current object ID to status bar in The main frame
			m_frame->m_wndStatusBar.SetPaneText(1, statusString.c_str(), 1);	
		}
	}

	return (int)msg.wParam;
}

void MFCMain::MenuFileQuit()
{
	//will post message to the message thread that will exit the application normally
	PostQuitMessage(0);
}

void MFCMain::MenuFileSaveTerrain()
{
	m_ToolSystem.onActionSaveTerrain();
}

void MFCMain::MenuConvertTextures()
{
	m_ToolTextureSelectDialogue.Create(IDD_DIALOG2);
	m_ToolTextureSelectDialogue.ShowWindow(SW_SHOW);
}

void MFCMain::MenuConvertModel()
{
	m_ToolModelSelectDialogue.Create(IDD_DIALOG3);
	m_ToolModelSelectDialogue.ShowWindow(SW_SHOW);
	//system("cmd.exe /k meshconvert cup._obj -cmo -flipz -y");
}

void MFCMain::MenuEditSelect()
{
	//SelectDialogue m_ToolSelectDialogue(NULL, &m_ToolSystem.m_sceneGraph);		//create our dialoguebox //modal constructor
	//m_ToolSelectDialogue.DoModal();	// start it up modal

	//modeless dialogue must be declared in the class.   If we do local it will go out of scope instantly and destroy itself
	m_ToolSelectDialogue.Create(IDD_DIALOG1);	//Start up modeless
	m_ToolSelectDialogue.ShowWindow(SW_SHOW);	//show modeless
	m_ToolSelectDialogue.SetObjectData(&m_ToolSystem.m_sceneGraph, &m_ToolSystem.m_selectedObject);
}

void MFCMain::MenuEditCreate()
{
	//From here we need to tell the game file to call the create command - basic version of this is copy and paste an object
	m_ToolObjectCreateDialogue.Create(IDD_DIALOG4);
	m_ToolObjectCreateDialogue.setToolSystem(&m_ToolSystem);
	m_ToolObjectCreateDialogue.ShowWindow(SW_SHOW);
	//m_ToolSystem.CreateNewGameObject();
}

void MFCMain::MenuEditObjectInspector()
{
	m_ToolObjectInspectorDialogue.Create(IDD_DIALOG5);
	m_ToolObjectInspectorDialogue.setToolSystem(&m_ToolSystem);
	m_ToolObjectInspectorDialogue.ShowWindow(SW_SHOW);
}

void MFCMain::MenuEditFlattenTerrain()
{
	m_ToolSystem.getGameSystem()->flattenTerrain();
}

void MFCMain::MenuEditGenerateNewTerrain()
{
	//m_ToolSystem.getGameSystem()->generateNewTerrain();
	m_ToolObjectGenerateTerrainDialogue.Create(IDD_DIALOG6);
	m_ToolObjectGenerateTerrainDialogue.setToolSystem(&m_ToolSystem);
	m_ToolObjectGenerateTerrainDialogue.ShowWindow(SW_SHOW);
}

void MFCMain::MenuEditLoadNewHeightmap()
{
	m_ToolObjectLoadNewHeightmapDialogue.Create(IDD_DIALOG7);
	m_ToolObjectLoadNewHeightmapDialogue.setToolSystem(&m_ToolSystem);
	m_ToolObjectLoadNewHeightmapDialogue.ShowWindow(SW_SHOW);
}

void MFCMain::ToolBarButton1()
{
	m_ToolSystem.onActionSave();
}

void MFCMain::ToolBarSaveTerrain()
{
	m_ToolSystem.getGameSystem()->saveTerrain();
	MessageBox(NULL, L"Terrain saved!", L"Success", MB_OK | MB_ICONINFORMATION);
}


MFCMain::MFCMain()
{
}


MFCMain::~MFCMain()
{
}
