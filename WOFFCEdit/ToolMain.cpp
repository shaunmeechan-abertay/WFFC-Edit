#include "ToolMain.h"
#include "resource.h"
#include <vector>
#include <sstream>

//
//ToolMain Class
ToolMain::ToolMain()
{

	m_currentChunk = 0;		//default value
	m_selectedObject = 0;	//initial selection ID
	m_sceneGraph.clear();	//clear the vector for the scenegraph
	m_databaseConnection = NULL;

	//zero input commands
	m_toolInputCommands.forward		= false;
	m_toolInputCommands.back		= false;
	m_toolInputCommands.right		= false;
	m_toolInputCommands.rotUp		= false;
	m_toolInputCommands.rotDown		= false;
	m_toolInputCommands.left		= false;
	m_toolInputCommands.rotRight	= false;
	m_toolInputCommands.rotLeft		= false;
	m_toolInputCommands.deleteObject		= false;
	m_toolInputCommands.UndoCommand		= false;
	m_toolInputCommands.RedoCommand		= false;
	m_toolInputCommands.mouse_X		= 0;
	m_toolInputCommands.mouse_Y		= 0;
	m_toolInputCommands.mouse_LB_Down		= false;
	m_toolInputCommands.mouse_LB_Up = false;
	m_toolInputCommands.multipick = false;
	m_toolInputCommands.copy = false;
	m_toolInputCommands.paste = false;
	m_toolInputCommands.focusOnObject = false;
	m_toolInputCommands.CTRL = false;
	
}


ToolMain::~ToolMain()
{
	sqlite3_close(m_databaseConnection);		//close the database connection
}


int ToolMain::getCurrentSelectionID()
{

	return m_selectedObject;
}

void ToolMain::onActionInitialise(HWND handle, int width, int height)
{
	//window size, handle etc for directX
	m_width		= width;
	m_height	= height;
	
	m_d3dRenderer.Initialize(handle, m_width, m_height);

	//database connection establish
	int rc;
	rc = sqlite3_open_v2("database/test.db",&m_databaseConnection, SQLITE_OPEN_READWRITE, NULL);

	if (rc) 
	{
		TRACE("Can't open database");
		//if the database cant open. Perhaps a more catastrophic error would be better here
	}
	else 
	{
		TRACE("Opened database successfully");
	}

	onActionLoad();
}

void ToolMain::onActionLoad()
{
	//load current chunk and objects into lists
	if (!m_sceneGraph.empty())		//is the vector empty
	{
		m_sceneGraph.clear();		//if not, empty it
	}

	//SQL
	int rc;
	char *sqlCommand;
	char *ErrMSG = 0;
	sqlite3_stmt *pResults;								//results of the query
	sqlite3_stmt *pResultsChunk;

	//OBJECTS IN THE WORLD
	//prepare SQL Text
	sqlCommand = "SELECT * from Objects";				//sql command which will return all records from the objects table.
	//Send Command and fill result object
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0 );
	
	//loop for each row in results until there are no more rows.  ie for every row in the results. We create and object
	while (sqlite3_step(pResults) == SQLITE_ROW)
	{	
		SceneObject newSceneObject;
		newSceneObject.ID = sqlite3_column_int(pResults, 0);
		newSceneObject.chunk_ID = sqlite3_column_int(pResults, 1);
		newSceneObject.model_path		= reinterpret_cast<const char*>(sqlite3_column_text(pResults, 2));
		newSceneObject.tex_diffuse_path = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 3));
		newSceneObject.posX = sqlite3_column_double(pResults, 4);
		newSceneObject.posY = sqlite3_column_double(pResults, 5);
		newSceneObject.posZ = sqlite3_column_double(pResults, 6);
		newSceneObject.rotX = sqlite3_column_double(pResults, 7);
		newSceneObject.rotY = sqlite3_column_double(pResults, 8);
		newSceneObject.rotZ = sqlite3_column_double(pResults, 9);
		newSceneObject.scaX = sqlite3_column_double(pResults, 10);
		newSceneObject.scaY = sqlite3_column_double(pResults, 11);
		newSceneObject.scaZ = sqlite3_column_double(pResults, 12);
		newSceneObject.render = sqlite3_column_int(pResults, 13);
		newSceneObject.collision = sqlite3_column_int(pResults, 14);
		newSceneObject.collision_mesh = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 15));
		newSceneObject.collectable = sqlite3_column_int(pResults, 16);
		newSceneObject.destructable = sqlite3_column_int(pResults, 17);
		newSceneObject.health_amount = sqlite3_column_int(pResults, 18);
		newSceneObject.editor_render = sqlite3_column_int(pResults, 19);
		newSceneObject.editor_texture_vis = sqlite3_column_int(pResults, 20);
		newSceneObject.editor_normals_vis = sqlite3_column_int(pResults, 21);
		newSceneObject.editor_collision_vis = sqlite3_column_int(pResults, 22);
		newSceneObject.editor_pivot_vis = sqlite3_column_int(pResults, 23);
		newSceneObject.pivotX = sqlite3_column_double(pResults, 24);
		newSceneObject.pivotY = sqlite3_column_double(pResults, 25);
		newSceneObject.pivotZ = sqlite3_column_double(pResults, 26);
		newSceneObject.snapToGround = sqlite3_column_int(pResults, 27);
		newSceneObject.AINode = sqlite3_column_int(pResults, 28);
		newSceneObject.audio_path = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 29));
		newSceneObject.volume = sqlite3_column_double(pResults, 30);
		newSceneObject.pitch = sqlite3_column_double(pResults, 31);
		newSceneObject.pan = sqlite3_column_int(pResults, 32);
		newSceneObject.one_shot = sqlite3_column_int(pResults, 33);
		newSceneObject.play_on_init = sqlite3_column_int(pResults, 34);
		newSceneObject.play_in_editor = sqlite3_column_int(pResults, 35);
		newSceneObject.min_dist = sqlite3_column_double(pResults, 36);
		newSceneObject.max_dist = sqlite3_column_double(pResults, 37);
		newSceneObject.camera = sqlite3_column_int(pResults, 38);
		newSceneObject.path_node = sqlite3_column_int(pResults, 39);
		newSceneObject.path_node_start = sqlite3_column_int(pResults, 40);
		newSceneObject.path_node_end = sqlite3_column_int(pResults, 41);
		newSceneObject.parent_id = sqlite3_column_int(pResults, 42);
		newSceneObject.editor_wireframe = sqlite3_column_int(pResults, 43);
		newSceneObject.name = reinterpret_cast<const char*>(sqlite3_column_text(pResults, 44));

		newSceneObject.light_type = sqlite3_column_int(pResults, 45);
		newSceneObject.light_diffuse_r = sqlite3_column_double(pResults, 46);
		newSceneObject.light_diffuse_g = sqlite3_column_double(pResults, 47);
		newSceneObject.light_diffuse_b = sqlite3_column_double(pResults, 48);
		newSceneObject.light_specular_r = sqlite3_column_double(pResults, 49);
		newSceneObject.light_specular_g = sqlite3_column_double(pResults, 50);
		newSceneObject.light_specular_b = sqlite3_column_double(pResults, 51);
		newSceneObject.light_spot_cutoff = sqlite3_column_double(pResults, 52);
		newSceneObject.light_constant = sqlite3_column_double(pResults, 53);
		newSceneObject.light_linear = sqlite3_column_double(pResults, 54);
		newSceneObject.light_quadratic = sqlite3_column_double(pResults, 55);
	

		//send completed object to scenegraph
		m_sceneGraph.push_back(newSceneObject);
	}

	//THE WORLD CHUNK
	//prepare SQL Text
	sqlCommand = "SELECT * from Chunks";				//sql command which will return all records from  chunks table. There is only one tho.
														//Send Command and fill result object
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResultsChunk, 0);


	sqlite3_step(pResultsChunk);
	m_chunk.ID = sqlite3_column_int(pResultsChunk, 0);
	m_chunk.name = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 1));
	m_chunk.chunk_x_size_metres = sqlite3_column_int(pResultsChunk, 2);
	m_chunk.chunk_y_size_metres = sqlite3_column_int(pResultsChunk, 3);
	m_chunk.chunk_base_resolution = sqlite3_column_int(pResultsChunk, 4);
	m_chunk.heightmap_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 5));
	m_chunk.tex_diffuse_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 6));
	m_chunk.tex_splat_alpha_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 7));
	m_chunk.tex_splat_1_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 8));
	m_chunk.tex_splat_2_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 9));
	m_chunk.tex_splat_3_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 10));
	m_chunk.tex_splat_4_path = reinterpret_cast<const char*>(sqlite3_column_text(pResultsChunk, 11));
	m_chunk.render_wireframe = sqlite3_column_int(pResultsChunk, 12);
	m_chunk.render_normals = sqlite3_column_int(pResultsChunk, 13);
	m_chunk.tex_diffuse_tiling = sqlite3_column_int(pResultsChunk, 14);
	m_chunk.tex_splat_1_tiling = sqlite3_column_int(pResultsChunk, 15);
	m_chunk.tex_splat_2_tiling = sqlite3_column_int(pResultsChunk, 16);
	m_chunk.tex_splat_3_tiling = sqlite3_column_int(pResultsChunk, 17);
	m_chunk.tex_splat_4_tiling = sqlite3_column_int(pResultsChunk, 18);


	//Process REsults into renderable
	m_d3dRenderer.BuildDisplayList(&m_sceneGraph);
	//build the renderable chunk 
	m_d3dRenderer.BuildDisplayChunk(&m_chunk);

}

void ToolMain::onActionSave()
{
	//Get the display list from the game incase it changed
	m_sceneGraph = m_d3dRenderer.getDisplayList();

	//SQL
	int rc;
	char *sqlCommand;
	char *ErrMSG = 0;
	int ErrCode = 0;
	sqlite3_stmt *pResults;								//results of the query
	
	//Before we do anything below, create a copy of the table
	sqlCommand = "CREATE TABLE backup AS SELECT * FROM Objects";
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0);
	ErrCode = sqlite3_step(pResults);

	switch (ErrCode)
	{
	case 0:
		//All good
		break;
	case 101:
		//All good
		break;
	case 100:
		//All good
		break;
	case 21:
		//Error here but it's cause the backup table already exists
		//We just want to clear it and copy in the new data
		sqlCommand = "DELETE FROM Objects";
		rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0);
		ErrCode = sqlite3_step(pResults);
		switch (ErrCode)
		{
		case 0:
			//All good
			copyObjectTableIntoBackup();
			break;
		case 101:
			//All good
			copyObjectTableIntoBackup();
			break;
		case 100:
			//All good
			copyObjectTableIntoBackup();
			break;
		default:
			MessageBox(NULL, L"An error occured within the backup table. Please manually delete it.", L"Error", MB_OK);
			return;
			break;
		}
		break;
	default:
		//SOMETHING WENT WRONG ABORT!
		MessageBox(NULL, L"An error occured creating the backup table!", L"Notification", MB_OK);
		return;
		break;
	}

	//OBJECTS IN THE WORLD Delete them all
	//prepare SQL Text
	//Maybe best to backup table before deleting the whole thing
	sqlCommand = "DELETE FROM Objects";	 //will delete the whole object table.   Slightly risky but hey. (Could this be improved with an ACID test?)
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0);
	ErrCode = sqlite3_step(pResults);

	switch (ErrCode)
	{
	case 0:
		//All good
		break;
	case 101:
		//All good
		break;
	case 100:
		//All good
		break;
	default:
		//SOMETHING WENT WRONG ABORT!
		MessageBox(NULL, L"An error occured!", L"Notification", MB_OK);
		return;
		break;
	}


	//Populate with our new objects
	std::wstring sqlCommand2;
	int numObjects = m_sceneGraph.size();	//Loop thru the scengraph.

	for (int i = 0; i < numObjects; i++)
	{
		std::stringstream command;
		command << "INSERT INTO Objects " 
			<<"VALUES(" << m_sceneGraph.at(i).ID << ","
			<< m_sceneGraph.at(i).chunk_ID  << ","
			<< "'" << m_sceneGraph.at(i).model_path <<"'" << ","
			<< "'" << m_sceneGraph.at(i).tex_diffuse_path << "'" << ","
			<< m_sceneGraph.at(i).posX << ","
			<< m_sceneGraph.at(i).posY << ","
			<< m_sceneGraph.at(i).posZ << ","
			<< m_sceneGraph.at(i).rotX << ","
			<< m_sceneGraph.at(i).rotY << ","
			<< m_sceneGraph.at(i).rotZ << ","
			<< m_sceneGraph.at(i).scaX << ","
			<< m_sceneGraph.at(i).scaY << ","
			<< m_sceneGraph.at(i).scaZ << ","
			<< m_sceneGraph.at(i).render << ","
			<< m_sceneGraph.at(i).collision << ","
			<< "'" << m_sceneGraph.at(i).collision_mesh << "'" << ","
			<< m_sceneGraph.at(i).collectable << ","
			<< m_sceneGraph.at(i).destructable << ","
			<< m_sceneGraph.at(i).health_amount << ","
			<< m_sceneGraph.at(i).editor_render << ","
			<< m_sceneGraph.at(i).editor_texture_vis << ","
			<< m_sceneGraph.at(i).editor_normals_vis << ","
			<< m_sceneGraph.at(i).editor_collision_vis << ","
			<< m_sceneGraph.at(i).editor_pivot_vis << ","
			<< m_sceneGraph.at(i).pivotX << ","
			<< m_sceneGraph.at(i).pivotY << ","
			<< m_sceneGraph.at(i).pivotZ << ","
			<< m_sceneGraph.at(i).snapToGround << ","
			<< m_sceneGraph.at(i).AINode << ","
			<< "'" << m_sceneGraph.at(i).audio_path << "'" << ","
			<< m_sceneGraph.at(i).volume << ","
			<< m_sceneGraph.at(i).pitch << ","
			<< m_sceneGraph.at(i).pan << ","
			<< m_sceneGraph.at(i).one_shot << ","
			<< m_sceneGraph.at(i).play_on_init << ","
			<< m_sceneGraph.at(i).play_in_editor << ","
			<< m_sceneGraph.at(i).min_dist << ","
			<< m_sceneGraph.at(i).max_dist << ","
			<< m_sceneGraph.at(i).camera << ","
			<< m_sceneGraph.at(i).path_node << ","
			<< m_sceneGraph.at(i).path_node_start << ","
			<< m_sceneGraph.at(i).path_node_end << ","
			<< m_sceneGraph.at(i).parent_id << ","
			<< m_sceneGraph.at(i).editor_wireframe << ","
			<< "'" << m_sceneGraph.at(i).name << "'" << ","

			<< m_sceneGraph.at(i).light_type << ","
			<< m_sceneGraph.at(i).light_diffuse_r << ","
			<< m_sceneGraph.at(i).light_diffuse_g << ","
			<< m_sceneGraph.at(i).light_diffuse_b << ","
			<< m_sceneGraph.at(i).light_specular_r << ","
			<< m_sceneGraph.at(i).light_specular_g << ","
			<< m_sceneGraph.at(i).light_specular_b << ","
			<< m_sceneGraph.at(i).light_spot_cutoff << ","
			<< m_sceneGraph.at(i).light_constant << ","
			<< m_sceneGraph.at(i).light_linear << ","
			<< m_sceneGraph.at(i).light_quadratic

			<< ")";
		std::string sqlCommand2 = command.str();
		rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand2.c_str(), -1, &pResults, 0);
		ErrCode = sqlite3_step(pResults);

		switch (ErrCode)
		{
		case 0:
			//All good
			break;
		case 101:
			//All good
			break;
		case 100:
			//All good
			break;
		default:
			//SOMETHING WENT WRONG, ATTEMPT FIX!
			MessageBox(NULL, L"An error occured! Table will now be restored to previous version. SAVE FAILED", L"Notification", MB_OK);
			sqlCommand = "DELETE FROM Objects";
			rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0);
			ErrCode = sqlite3_step(pResults);
			switch (ErrCode)
			{
			case 0:
				//All good
				MessageBox(NULL, L"Table succesfully cleared. Restoring data", L"Notification", MB_OK);
				copyBackupTableIntoObjects();
				break;
			case 101:
				//All good
				MessageBox(NULL, L"Table succesfully cleared. Restoring data", L"Notification", MB_OK);
				copyBackupTableIntoObjects();
				break;
			case 100:
				//All good
				MessageBox(NULL, L"Table succesfully cleared. Restoring data", L"Notification", MB_OK);
				copyBackupTableIntoObjects();
				break;
			default:
				//SOMETHING WENT VERY WRONG, DROP TABLE AND RESTORE FROM BACKUP (This is the nuclear option)!
				MessageBox(NULL, L"Critical error! Unable to clear table. Table will now be deleted and re-made", L"Error", MB_OK);
				sqlCommand = "DROP TABLE Objects";
				rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0);
				ErrCode = sqlite3_step(pResults);
				switch (ErrCode)
				{
				case 0:
					//All good
					remakeObjectTable();
					break;
				case 101:
					//All good
					remakeObjectTable();
					break;
				case 100:
					//All good
					remakeObjectTable();
					break;
				default:
					//SOMETHING WENT WRONG ABORT!
					MessageBox(NULL, L"Unrecoverable error encountered. Unable to re-create table. The database may be corrupted. Please attempt manual repairs.", L"Error", MB_OK);
					return;
					break;
				}
				break;
			}
			//Table will either be fixed here or unrecoverable
			return;
			break;
		}
	}
	MessageBox(NULL, L"Objects Saved", L"Notification", MB_OK);
	sqlCommand = "DROP TABLE backup";
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0);
	ErrCode = sqlite3_step(pResults);
	switch (ErrCode)
	{
	case 0:
		//All good
		break;
	case 101:
		//All good
		break;
	case 100:
		//All good
		break;
	default:
		//SOMETHING WENT WRONG ABORT!
		MessageBox(NULL, L"ERROR: Unable to delete backup table. Please remove manually.", L"Notification", MB_OK);
		return;
		break;
	}

}

void ToolMain::onActionSaveTerrain()
{
	m_d3dRenderer.SaveDisplayChunk(&m_chunk);
}

void ToolMain::Tick(MSG *msg)
{
	//do we have a selection
	//do we have a mode
	//are we clicking / dragging /releasing
	//has something changed
		//update Scenegraph
		//add to scenegraph
		//resend scenegraph to Direct X renderer

	//Renderer Update Call
	m_d3dRenderer.Tick(&m_toolInputCommands);

	if (m_toolInputCommands.mouse_LB_Down)
	{
		//m_selectedObject = m_d3dRenderer.MousePicking();
		//Call click and drag function
		m_d3dRenderer.clickAndDrag();
	}

	if (m_toolInputCommands.mouse_RB_Down)
	{
		//User is trying to move camera using mouse
		m_d3dRenderer.mouseCameraMovement();
	}
}

void ToolMain::UpdateInput(MSG * msg)
{

	switch (msg->message)
	{
		//Global inputs,  mouse position and keys etc
	case WM_KEYDOWN:
		m_keyArray[msg->wParam] = true;
		break;

	case WM_KEYUP:
		m_keyArray[msg->wParam] = false;
		break;

	case WM_MOUSEMOVE:
		//Update the mouse X and Y which will be sent through to the Renderer.
		m_toolInputCommands.mouse_X = GET_X_LPARAM(msg->lParam);
		m_toolInputCommands.mouse_Y = GET_Y_LPARAM(msg->lParam);
		break;

	case WM_LBUTTONDOWN:	//mouse button down,  you will probably need to check when its up too
		//set some flag for the mouse button in inputcommands
		//mouse left pressed
		m_toolInputCommands.mouse_LB_Down = true;
		m_selectedObject = m_d3dRenderer.MousePicking();
		m_d3dRenderer.checkForDragArrow();
		//See if the user clicked a drag Arrow
		m_toolInputCommands.mouse_LB_Up = false;
		break;

	case WM_LBUTTONUP:
		m_toolInputCommands.mouse_LB_Up = true;
		m_toolInputCommands.mouse_LB_Down = false;
		//A drag finished, create a UndoMove actions
		m_d3dRenderer.dragFinished();
		break;

	case WM_RBUTTONDOWN:
		//Right mouse button being else down
		m_toolInputCommands.mouse_RB_Down = true;
		m_toolInputCommands.mouse_RB_Up = false;
		break;

	case WM_RBUTTONUP:
		//Right mouse button lifted
		m_toolInputCommands.mouse_RB_Up = true;
		m_toolInputCommands.mouse_RB_Down = false;
		break;
	}
	//here we update all the actual app functionality that we want.  This information will either be used int toolmain, or sent down to the renderer (Camera movement etc
	//WASD movement
	if (m_keyArray['W'])
	{
		m_toolInputCommands.forward = true;
	}
	else m_toolInputCommands.forward = false;
	
	if (m_keyArray['S'])
	{
		m_toolInputCommands.back = true;
	}
	else m_toolInputCommands.back = false;
	if (m_keyArray['A'])
	{
		m_toolInputCommands.left = true;
	}
	else m_toolInputCommands.left = false;

	if (m_keyArray['D'])
	{
		m_toolInputCommands.right = true;
	}
	else m_toolInputCommands.right = false;
	//rotation
	if (m_keyArray['E'])
	{
		m_toolInputCommands.rotRight = true;
	}
	else m_toolInputCommands.rotRight = false;

	if (m_keyArray['Q'])
	{
		m_toolInputCommands.rotLeft = true;
	}
	else m_toolInputCommands.rotLeft = false;
	if (m_keyArray['O'])
	{
		m_toolInputCommands.rotUp = true;
	}
	else m_toolInputCommands.rotUp = false;
	if (m_keyArray['P'])
	{
		m_toolInputCommands.rotDown = true;
	}
	else m_toolInputCommands.rotDown = false;

	//Delete
	if (m_keyArray[VK_DELETE])
	{
		m_toolInputCommands.deleteObject = true;
	}
	else m_toolInputCommands.deleteObject = false;

	//Undo 
	if (m_keyArray['Z'] && m_keyArray[VK_CONTROL])
	{
		m_toolInputCommands.UndoCommand = true;
	}
	else m_toolInputCommands.UndoCommand = false;
	//Redo
	if (m_keyArray['Y'] && m_keyArray[VK_CONTROL])
	{
		m_toolInputCommands.RedoCommand = true;
	}
	else m_toolInputCommands.RedoCommand = false;
	//Multipick
	if (m_keyArray[VK_CONTROL])
	{
		m_toolInputCommands.multipick = true;
	}
	else m_toolInputCommands.multipick = false;
	//Copy
	if (m_keyArray['C'] && m_keyArray[VK_CONTROL])
	{
		m_toolInputCommands.copy = true;
	}
	else m_toolInputCommands.copy = false;
	//Paste
	if (m_keyArray['V'] && m_keyArray[VK_CONTROL])
	{
		m_toolInputCommands.paste = true;
	}
	else m_toolInputCommands.paste = false;	
	//Focus on an object
	if (m_keyArray['F'])
	{
		m_toolInputCommands.focusOnObject = true;
	}
	else m_toolInputCommands.focusOnObject = false;	
	//Alt
	if (m_keyArray[VK_CONTROL])
	{
		m_toolInputCommands.CTRL = true;
	}
	else
	{
		m_toolInputCommands.CTRL = false;
	}
}

void ToolMain::CreateNewGameObject(CString texture, CString models, DirectX::XMVECTOR position, DirectX::XMVECTOR scale, DirectX::XMVECTOR rotation)
{
	//Convert the CStrings to a strings
	//This get's around a weird LINK2019 error that happens if this sends a CString and game takes that in
	CT2CA convert(models);
	std::string s(convert);

	CT2CA convertedTexturePath(texture);
	std::string textureFile(convertedTexturePath);
	m_d3dRenderer.CreateNewObject(textureFile,s, position,scale,rotation);

}

Game* ToolMain::getGameSystem()
{
	return &m_d3dRenderer;
}

std::vector<DisplayObject*> ToolMain::getSelectedObjects()
{
	return m_d3dRenderer.getSelectedObjects();
}

DisplayObject* ToolMain::getSelectedGameObject()
{
	return m_d3dRenderer.getSelectedObject();
}

void ToolMain::remakeObjectTable()
{
	int rc;
	char* sqlCommand;
	char* ErrMSG = 0;
	int ErrCode = 0;
	sqlite3_stmt* pResults;								//results of the query

	//Before we do anything below, create a copy of the table
	sqlCommand = "CREATE TABLE Objects AS SELECT * FROM backup";
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0);
	ErrCode = sqlite3_step(pResults);
	switch (ErrCode)
	{
	case 0:
		//All good
		MessageBox(NULL, L"Success! Table was restored from backup.", L"Notification", MB_OK);
		break;
	case 101:
		//All good
		MessageBox(NULL, L"Success! Table was restored from backup.", L"Notification", MB_OK);
		break;
	case 100:
		//All good
		MessageBox(NULL, L"Success! Table was restored from backup.", L"Notification", MB_OK);
		break;
	default:
		//If this fails we need the user to fix this manually
		MessageBox(NULL, L"Unrecoverable error encountered. Unable to re-create table. The database may be corrupted. Please attempt manual repairs.", L"Error", MB_OK);
		return;
		break;
	}
}

void ToolMain::copyBackupTableIntoObjects()
{
	int rc;
	char* sqlCommand;
	char* ErrMSG = 0;
	int ErrCode = 0;
	sqlite3_stmt* pResults;								//results of the query

	sqlCommand = "INSERT INTO Objects SELECT * FROM backup";
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0);
	ErrCode = sqlite3_step(pResults);
	switch (ErrCode)
	{
	case 0:
		//All good
		MessageBox(NULL, L"Success! Table was restored from backup.", L"Notification", MB_OK);
		break;
	case 101:
		//All good
		MessageBox(NULL, L"Success! Table was restored from backup.", L"Notification", MB_OK);
		break;
	case 100:
		//All good
		MessageBox(NULL, L"Success! Table was restored from backup.", L"Notification", MB_OK);
		break;
	default:
		//SOMETHING WENT WRONG ABORT!
		MessageBox(NULL, L"Unrecoverable error encountered. Unable to re-create table. The database may be corrupted. Please attempt manual repairs.", L"Error", MB_OK);
		return;
		break;
	}
}

void ToolMain::copyObjectTableIntoBackup()
{
	int rc;
	char* sqlCommand;
	char* ErrMSG = 0;
	int ErrCode = 0;
	sqlite3_stmt* pResults;								//results of the query

	sqlCommand = "INSERT INTO backup SELECT * FROM Objects";
	rc = sqlite3_prepare_v2(m_databaseConnection, sqlCommand, -1, &pResults, 0);
	ErrCode = sqlite3_step(pResults);
	switch (ErrCode)
	{
	case 0:
		//All good
		break;
	case 101:
		//All good
		break;
	case 100:
		//All good
		break;
	default:
		//SOMETHING WENT WRONG ABORT!
		MessageBox(NULL, L"Unrecoverable error encountered. Unable to copy into backup table. The table may be corrupted. Please manually delete it.", L"Error", MB_OK);
		return;
		break;
	}
}
