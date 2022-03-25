//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "DisplayObject.h"
#include <string>


using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game()

{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
	m_displayList.clear();
	
	//initial Settings
	//modes
	m_grid = false;

	//functional
	m_movespeed = 0.30;
	m_camRotRate = 3.0;

	//camera
	m_camPosition.x = 0.0f;
	m_camPosition.y = 3.7f;
	m_camPosition.z = -3.5f;

	m_camOrientation.x = 0;
	m_camOrientation.y = 0;
	m_camOrientation.z = 0;

	m_camLookAt.x = 0.0f;
	m_camLookAt.y = 0.0f;
	m_camLookAt.z = 0.0f;

	m_camLookDirection.x = 0.0f;
	m_camLookDirection.y = 0.0f;
	m_camLookDirection.z = 0.0f;

	m_camRight.x = 0.0f;
	m_camRight.y = 0.0f;
	m_camRight.z = 0.0f;

	m_camOrientation.x = 0.0f;
	m_camOrientation.y = 0.0f;
	m_camOrientation.z = 0.0f;

}

Game::~Game()
{

#ifdef DXTK_AUDIO
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_gamePad = std::make_unique<GamePad>();

    m_keyboard = std::make_unique<Keyboard>();

    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(window);

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

	GetClientRect(window, &m_ScreenDimensions);

#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    m_effect2->Play();
#endif
}

void Game::SetGridState(bool state)
{
	m_grid = state;
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick(InputCommands *Input)
{
	//copy over the input commands so we have a local version to use elsewhere.
	m_InputCommands = *Input;
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	//TODO  any more complex than this, and the camera should be abstracted out to somewhere else
	//camera motion is on a plane, so kill the 7 component of the look direction
	Vector3 planarMotionVector = m_camLookDirection;
	planarMotionVector.y = 0.0;

	if (m_InputCommands.rotRight)
	{
		m_camOrientation.y -= m_camRotRate;
	}
	if (m_InputCommands.rotLeft)
	{
		m_camOrientation.y += m_camRotRate;
	}
	if (m_InputCommands.rotUp)
	{
		m_camOrientation.x -= m_camRotRate;
	}
	if (m_InputCommands.rotDown)
	{
		m_camOrientation.x += m_camRotRate;
	}


	//create look direction from Euler angles in m_camOrientation
	if (shouldResetOrientation == false)
	{
		m_camLookDirection.x = cos((m_camOrientation.y) * 3.1415 / 180) * cos((m_camOrientation.x) * 3.1415 / 180);

		m_camLookDirection.y = sin((m_camOrientation.x) * 3.1415 / 180);

		m_camLookDirection.z = sin((m_camOrientation.y) * 3.1415 / 180) * cos((m_camOrientation.x) * 3.1415 / 180);
	}
	else
	{
		shouldResetOrientation = false;
	}

	m_camLookDirection.Normalize();

	//create right vector from look Direction
	m_camLookDirection.Cross(Vector3::UnitY, m_camRight);

	//process input and update stuff
	if (m_InputCommands.forward)
	{	
		m_camPosition += m_camLookDirection*m_movespeed;
	}
	if (m_InputCommands.back)
	{
		m_camPosition -= m_camLookDirection*m_movespeed;
	}
	if (m_InputCommands.right)
	{
		m_camPosition += m_camRight*m_movespeed;
	}
	if (m_InputCommands.left)
	{
		m_camPosition -= m_camRight*m_movespeed;
	}
	if (m_InputCommands.deleteObject && inputDown == false)
	{
		inputDown = true;
		DeleteCommand* deleteCommand =  new DeleteCommand;
		if (selectedObjects.empty() == false)
		{
			deleteCommand->performAction(m_displayList, selectedObjects);
			selectedObjects.clear();
		}
		else
		{
 			deleteCommand->performAction(m_displayList, selectedObject->m_ID);
		}
		Commands *command = deleteCommand;
		commandList.push_back(command);
	}

	if (m_InputCommands.UndoCommand && inputDown == false)
	{
		inputDown = true;
		undoAction();
	}

	if (m_InputCommands.RedoCommand && inputDown == false)
	{
		inputDown = true;
		RedoAction();
	}

	if (m_InputCommands.copy && inputDown == false)
	{
		inputDown = true;
		copyObject();
	}	
	
	if (m_InputCommands.paste && inputDown == false)
	{
		inputDown = true;
		pasteObject();
	}

	if (m_InputCommands.focusOnObject && inputDown == false)
	{
		inputDown = true;
		focusOnItem();
	}

	//This might not work long term (this really is getting bad now)
	if (m_InputCommands.UndoCommand == false && m_InputCommands.deleteObject == false && m_InputCommands.RedoCommand == false && m_InputCommands.copy == false && m_InputCommands.paste == false && m_InputCommands.focusOnObject == false)
	{
		inputDown = false;
	}

	//update lookat point
	m_camLookAt = m_camPosition + m_camLookDirection;

	//apply camera vectors
    m_view = Matrix::CreateLookAt(m_camPosition, m_camLookAt, Vector3::UnitY);

    m_batchEffect->SetView(m_view);
    m_batchEffect->SetWorld(Matrix::Identity);
	m_displayChunk.m_terrainEffect->SetView(m_view);
	m_displayChunk.m_terrainEffect->SetWorld(Matrix::Identity);

#ifdef DXTK_AUDIO
    m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                m_effect1->Play(true);
            }
        }
        else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }
#endif

   
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

	if (m_grid)
	{
		// Draw procedurally generated dynamic grid
		const XMVECTORF32 xaxis = { 512.f, 0.f, 0.f };
		const XMVECTORF32 yaxis = { 0.f, 0.f, 512.f };
		DrawGrid(xaxis, yaxis, g_XMZero, 512, 512, Colors::Gray);
	}
	//CAMERA POSITION ON HUD - DON'T MOVE THIS! WHILE IT IS BROKEN (RENDER BEHIND EVERYTHING) MOVINING IT JUST CAUSES CHAOS
	//SOME OBJECTS WON'T RENDER OTHER WILL, THE TERRAIN HAS NO TEXTURE - JUST BAD!
	//TODO: This is probably fixable so try fix this if theres time.

	m_sprites->Begin();
	WCHAR   Buffer[256];
	std::wstring var = L"Cam X: " + std::to_wstring(m_camPosition.x) + L"Cam Z: " + std::to_wstring(m_camPosition.z);
	m_font->DrawString(m_sprites.get(), var.c_str() , XMFLOAT2(100, 10), Colors::Yellow);
	m_sprites->End();

	//RENDER OBJECTS FROM SCENEGRAPH
	int numRenderObjects = m_displayList.size();
	for (int i = 0; i < numRenderObjects; i++)
	{
		m_deviceResources->PIXBeginEvent(L"Draw model");
		const XMVECTORF32 scale = { m_displayList[i].m_scale.x, m_displayList[i].m_scale.y, m_displayList[i].m_scale.z };
		const XMVECTORF32 translate = { m_displayList[i].m_position.x, m_displayList[i].m_position.y, m_displayList[i].m_position.z };

		//convert degrees into radians for rotation matrix
		XMVECTOR rotate = Quaternion::CreateFromYawPitchRoll(m_displayList[i].m_orientation.y *3.1415 / 180,
															m_displayList[i].m_orientation.x *3.1415 / 180,
															m_displayList[i].m_orientation.z *3.1415 / 180);

		XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);

		m_displayList[i].m_model->Draw(context, *m_states, local, m_view, m_projection, m_displayList[i].m_wireframe);	//last variable in draw,  make TRUE for wireframe

		m_deviceResources->PIXEndEvent();
	}

	numRenderObjects = m_dragArrowList.size();
	for(int i = 0; i < numRenderObjects; i++)
	{
		m_deviceResources->PIXBeginEvent(L"Draw model");
		const XMVECTORF32 scale = { m_dragArrowList[i].m_scale.x, m_dragArrowList[i].m_scale.y, m_dragArrowList[i].m_scale.z };
		const XMVECTORF32 translate = { m_dragArrowList[i].m_position.x, m_dragArrowList[i].m_position.y, m_dragArrowList[i].m_position.z };

		//convert degrees into radians for rotation matrix
		XMVECTOR rotate = Quaternion::CreateFromYawPitchRoll(m_dragArrowList[i].m_orientation.y * 3.1415 / 180,
			m_dragArrowList[i].m_orientation.x * 3.1415 / 180,
			m_dragArrowList[i].m_orientation.z * 3.1415 / 180);

		XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);

		m_dragArrowList[i].m_model->Draw(context, *m_states, local, m_view, m_projection, m_dragArrowList[i].m_wireframe);	//last variable in draw,  make TRUE for wireframe

		m_deviceResources->PIXEndEvent();
	}

    m_deviceResources->PIXEndEvent();

	//RENDER TERRAIN - To fix the issue of the terrain texture dissaperaing, just hid a box in the world? Player never sees it so can't delete it! (Thanks Matt!)
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(),0);
	context->RSSetState(m_states->CullNone());
//	context->RSSetState(m_states->Wireframe());		//uncomment for wireframe

	//Render the batch,  This is handled in the Display chunk becuase it has the potential to get complex
	m_displayChunk.RenderBatch(m_deviceResources);

    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetBackBufferRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

void XM_CALLCONV Game::DrawGrid(FXMVECTOR xAxis, FXMVECTOR yAxis, FXMVECTOR origin, size_t xdivs, size_t ydivs, GXMVECTOR color)
{
    m_deviceResources->PIXBeginEvent(L"Draw grid");

    auto context = m_deviceResources->GetD3DDeviceContext();
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthNone(), 0);
    context->RSSetState(m_states->CullCounterClockwise());

    m_batchEffect->Apply(context);

    context->IASetInputLayout(m_batchInputLayout.Get());

    m_batch->Begin();

    xdivs = std::max<size_t>(1, xdivs);
    ydivs = std::max<size_t>(1, ydivs);

    for (size_t i = 0; i <= xdivs; ++i)
    {
        float fPercent = float(i) / float(xdivs);
        fPercent = (fPercent * 2.0f) - 1.0f;
        XMVECTOR vScale = XMVectorScale(xAxis, fPercent);
        vScale = XMVectorAdd(vScale, origin);

        VertexPositionColor v1(XMVectorSubtract(vScale, yAxis), color);
        VertexPositionColor v2(XMVectorAdd(vScale, yAxis), color);
        m_batch->DrawLine(v1, v2);
    }

    for (size_t i = 0; i <= ydivs; i++)
    {
        float fPercent = float(i) / float(ydivs);
        fPercent = (fPercent * 2.0f) - 1.0f;
        XMVECTOR vScale = XMVectorScale(yAxis, fPercent);
        vScale = XMVectorAdd(vScale, origin);

        VertexPositionColor v1(XMVectorSubtract(vScale, xAxis), color);
        VertexPositionColor v2(XMVectorAdd(vScale, xAxis), color);
        m_batch->DrawLine(v1, v2);
    }

    m_batch->End();

    m_deviceResources->PIXEndEvent();
}

void Game::setSelectedObject(DisplayObject* newObject)
{
	if (selectedObject != NULL)
	{
		selectedObject->m_wireframe = false;
		selectedObject->m_selected = false;
		//Clear all arrows
		m_dragArrowList.clear();
	}
	selectedObject = newObject;

	//Now we need to spawn the selection arrows around the object in 6 places
	for (unsigned int i = 0; i < 6; i++)
	{
		DragArrow newArrow;
		switch (i)
		{
		case 0:
			//Up
			newArrow.setup(selectedObject->m_position.x, 1, selectedObject->m_position.z, 0, 0, 0, m_deviceResources, *m_fxFactory);
			m_dragArrowList.push_back(newArrow);
			break;
		case 1:
			//Down (should also rotate this to face down once actual model is in)
			newArrow.setup(selectedObject->m_position.x, -1, selectedObject->m_position.z, 0, 0, 0, m_deviceResources, *m_fxFactory);
			m_dragArrowList.push_back(newArrow);
			break;
		case 2:
			//Left 
			newArrow.setup(selectedObject->m_position.x - 1, selectedObject->m_position.y, selectedObject->m_position.z, 0, 0, 90, m_deviceResources, *m_fxFactory);
			m_dragArrowList.push_back(newArrow);
			break;
		case 3:
			//Right
			newArrow.setup(selectedObject->m_position.x + 1, selectedObject->m_position.y, selectedObject->m_position.z, 0, 0, -90, m_deviceResources, *m_fxFactory);
			m_dragArrowList.push_back(newArrow);
			break;
		case 4:
			//back
			newArrow.setup(selectedObject->m_position.x, selectedObject->m_position.y, selectedObject->m_position.z - 1, -90, 0, 0, m_deviceResources, *m_fxFactory);
			m_dragArrowList.push_back(newArrow);
			break;
		case 5:
			//forward
			newArrow.setup(selectedObject->m_position.x, selectedObject->m_position.y, selectedObject->m_position.z + 1, 90, 0, 0, m_deviceResources, *m_fxFactory);
			m_dragArrowList.push_back(newArrow);
			break;
		default:
			break;
		}
	}
}
void Game::setSelectedObjects(std::vector<DisplayObject> newObjects)
{
	if (newObjects.empty() == true)
	{
		//If we are sent nothing, do nothing
		return;
	}
	if (selectedObjects.empty() == false)
	{
		for (int i = 0; i < selectedObjects.size(); i++)
		{
			//Find the object in the display list
			for (int j = 0; j < m_displayList.size(); j++)
			{
				if (selectedObjects[i].m_ID == m_displayList[j].m_ID)
				{
					m_displayList[j].m_wireframe = false;
					m_displayList[j].m_selected = false;
				}
			}
		}
		selectedObjects.clear();
	}
		for (int i = 0; i < newObjects.size(); i++)
		{
			newObjects[i].m_wireframe = true;
			newObjects[i].m_selected = true;
			selectedObjects.push_back(newObjects[i]);
		}
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
    m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
    m_audEngine->Resume();
#endif
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

void Game::BuildDisplayList(std::vector<SceneObject> * SceneGraph)
{
	auto device = m_deviceResources->GetD3DDevice();
	auto devicecontext = m_deviceResources->GetD3DDeviceContext();

	if (!m_displayList.empty())		//is the vector empty
	{
		m_displayList.clear();		//if not, empty it
	}

	//for every item in the scenegraph
	int numObjects = SceneGraph->size();
	for (int i = 0; i < numObjects; i++)
	{
		
		//create a temp display object that we will populate then append to the display list.
		DisplayObject newDisplayObject;
		
		//load model
		std::wstring modelwstr = StringToWCHART(SceneGraph->at(i).model_path);							//convect string to Wchar
		newDisplayObject.m_model = Model::CreateFromCMO(device, modelwstr.c_str(), *m_fxFactory, true);	//get DXSDK to load model "False" for LH coordinate system (maya)

		//Load Texture
		std::wstring texturewstr = StringToWCHART(SceneGraph->at(i).tex_diffuse_path);								//convect string to Wchar
		HRESULT rs;
		rs = CreateDDSTextureFromFile(device, texturewstr.c_str(), nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource

		//if texture fails.  load error default
		if (rs)
		{
			CreateDDSTextureFromFile(device, L"database/data/Error.dds", nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource
		}

		//apply new texture to models effect
		newDisplayObject.m_model->UpdateEffects([&](IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
		{	
			auto lights = dynamic_cast<BasicEffect*>(effect);
			if (lights)
			{
				lights->SetTexture(newDisplayObject.m_texture_diffuse);			
			}
		});

		//Set texture and model path
		newDisplayObject.m_texturePath = SceneGraph->at(i).tex_diffuse_path;
		newDisplayObject.m_modelPath = SceneGraph->at(i).model_path;

		//set position
		newDisplayObject.m_position.x = SceneGraph->at(i).posX;
		newDisplayObject.m_position.y = SceneGraph->at(i).posY;
		newDisplayObject.m_position.z = SceneGraph->at(i).posZ;
		
		//setorientation
		newDisplayObject.m_orientation.x = SceneGraph->at(i).rotX;
		newDisplayObject.m_orientation.y = SceneGraph->at(i).rotY;
		newDisplayObject.m_orientation.z = SceneGraph->at(i).rotZ;

		//set scale
		newDisplayObject.m_scale.x = SceneGraph->at(i).scaX;
		newDisplayObject.m_scale.y = SceneGraph->at(i).scaY;
		newDisplayObject.m_scale.z = SceneGraph->at(i).scaZ;

		//set wireframe / render flags
		newDisplayObject.m_render		= SceneGraph->at(i).editor_render;
		newDisplayObject.m_wireframe	= SceneGraph->at(i).editor_wireframe;
		//newDisplayObject.m_wireframe	= true;

		newDisplayObject.m_light_type		= SceneGraph->at(i).light_type;
		newDisplayObject.m_light_diffuse_r	= SceneGraph->at(i).light_diffuse_r;
		newDisplayObject.m_light_diffuse_g	= SceneGraph->at(i).light_diffuse_g;
		newDisplayObject.m_light_diffuse_b	= SceneGraph->at(i).light_diffuse_b;
		newDisplayObject.m_light_specular_r = SceneGraph->at(i).light_specular_r;
		newDisplayObject.m_light_specular_g = SceneGraph->at(i).light_specular_g;
		newDisplayObject.m_light_specular_b = SceneGraph->at(i).light_specular_b;
		newDisplayObject.m_light_spot_cutoff = SceneGraph->at(i).light_spot_cutoff;
		newDisplayObject.m_light_constant	= SceneGraph->at(i).light_constant;
		newDisplayObject.m_light_linear		= SceneGraph->at(i).light_linear;
		newDisplayObject.m_light_quadratic	= SceneGraph->at(i).light_quadratic;

		//Assign ID
		newDisplayObject.m_ID = SceneGraph->at(i).ID;
		
		m_displayList.push_back(newDisplayObject);
		
	}
		
		
		
}

void Game::BuildDisplayChunk(ChunkObject * SceneChunk)
{
	//populate our local DISPLAYCHUNK with all the chunk info we need from the object stored in toolmain
	//which, to be honest, is almost all of it. Its mostly rendering related info so...
	m_displayChunk.PopulateChunkData(SceneChunk);		//migrate chunk data
	m_displayChunk.LoadHeightMap(m_deviceResources);
	m_displayChunk.m_terrainEffect->SetProjection(m_projection);
	m_displayChunk.InitialiseBatch();
}

void Game::SaveDisplayChunk(ChunkObject * SceneChunk)
{
	m_displayChunk.SaveHeightMap();			//save heightmap to file.
}

int Game::MousePicking()
{
	int selectedID = -1;
	float pickedDistance = 0;
	//Set the float to the max possible distance as we will reduce this to the closes object
	//Could have used a random high number but this number is massive (3.40282e+38) so nothing with ever be further than it (or shouldn't!)
	float closestDistance = std::numeric_limits<float>::max();

	//setup near and far planes of frustum with mouse X and mouse Y passed
	//down from Toolmain.
	//They may look the same but note the difference in Z
	const XMVECTOR nearSource = XMVectorSet(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, 0.0f, 1.0f);
	const XMVECTOR farSource = XMVectorSet(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, 1.0f, 1.0f);

	//Loop through entire display list of objects and pick with each in turn.
	for (int i = 0; i < m_displayList.size(); i++)
	{
		//Get the scale factor and translation of the object
		const XMVECTORF32 scale = { m_displayList[i].m_scale.x,
			m_displayList[i].m_scale.y, m_displayList[i].m_scale.z };
		const XMVECTORF32 translate = { m_displayList[i].m_position.x,
			m_displayList[i].m_position.y, m_displayList[i].m_position.z };
		//Convert eular angles into a quaterion for the rotation of the object
		XMVECTOR rotate = Quaternion::CreateFromYawPitchRoll(m_displayList[i].m_orientation.y * 3.1415 / 180, 
			m_displayList[i].m_orientation.x * 3.1415/180,
			m_displayList[i].m_orientation.z * 3.1415/180);
		//Create set the matrix of the selected object in the world based on the translation,scale and roation
		XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);
		//Unproject the points on the near and far plane, with respect to the matrix we just created
		XMVECTOR nearPoint = XMVector3Unproject(nearSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);
		XMVECTOR farPoint = XMVector3Unproject(farSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);

		//Turn the transformed points into our picking vector
		XMVECTOR pickingVector = farPoint - nearPoint;
		pickingVector = XMVector3Normalize(pickingVector);

		//Loop through mesh list for object
		for (int y = 0; y < m_displayList[i].m_model.get()->meshes.size(); y++)
		{
			//Checking for ray intersection
			if (m_displayList[i].m_model.get()->meshes[y]->boundingBox.Intersects(nearPoint, pickingVector, pickedDistance) && pickedDistance < closestDistance)
			{
				m_displayList[i].m_wireframe = true;

				if (m_InputCommands.multipick && m_displayList[i].m_selected == false)
				{
					m_displayList[i].m_selected = true;
					if (selectedObject != NULL)
					{
						selectedObjects.push_back(*selectedObject);
						//Need to make sure selected object is NULL'd other wise the same object will be added a lot
						//setSelectedObject(NULL);
					}
					selectedObjects.push_back(m_displayList[i]);
				}
				else if (m_InputCommands.multipick && m_displayList[i].m_selected == true)
				{
					m_displayList[i].m_selected = false;
					//Got to find object in selected objects list
					for (int j = 0; j < selectedObjects.size(); j++)
					{
						if (m_displayList[i].m_ID == selectedObjects.at(j).m_ID)
						{
							//Found object, remove
							selectedObjects.erase(selectedObjects.begin() + j);
							break;
						}
					}
					m_displayList[i].m_wireframe = false;
				}
				//Single object pick
				else
				{
					//If we have selected objects - deselect them
					if (selectedObjects.empty() == false)
					{
						//This seems inefficient, why not find the ones we need and unwireframe them?
						for (int j = 0; j < m_displayList.size(); j++)
						{
							m_displayList[j].m_wireframe = false;
						}
						selectedObjects.clear();
					}
					selectedID = m_displayList[i].m_ID;
					if (selectedObject == NULL)
					{
						setSelectedObject(&m_displayList[i]);
					}

					//If we have a selected object and the user didn't click it
					if (selectedObject != NULL && selectedID != selectedObject->m_ID)
					{
						setSelectedObject(&m_displayList[i]);
					}
				}

				closestDistance = pickedDistance;
			}
		}
	}
	//If we got a hit. Return it.
	setID(selectedID);
	return selectedID;
}

void Game::setID(int newID)
{
	ID = newID;
}

void Game::copyObject()
{
	if (selectedObjects.empty() == false)
	{
		copyCommand.performAction(selectedObjects);
	}
	else
	{
		if (selectedObject != NULL)
		{
			copyCommand.performAction(selectedObject);
		}
		else
		{
			return;
		}
	}
}

void Game::pasteObject()
{
	CreateCommand* createCommand = new CreateCommand;
	if (copyCommand.getCopiedObjects().empty() == false)
	{
		createCommand->performAction(m_displayList, copyCommand.getCopiedObjects(), m_fxFactory, true);
		setSelectedObjects(createCommand->getCreatedObjects());
	}
	else
	{
		createCommand->performAction(m_displayList, copyCommand.getCopiedObject(), m_fxFactory, true);
		setSelectedObject(&m_displayList.at(createCommand->getCreatedObject().m_ID - 1));
	}
	Commands* command = createCommand;
	commandList.push_back(command);
}

void Game::focusOnItem()
{
	//For now focus on a single item
	if (selectedObject == NULL)
	{
	}
	else
	{
		//Move camera to object position
		shouldResetOrientation = true;
		m_camPosition = selectedObject->m_position + Vector3(-5, 0, 5);

		//Calculate angle between the camera and the object
		//Create a direction vector
		Vector3 dir = selectedObject->m_position - m_camPosition;
		//Get arctan of the vector
		float angle = atan2f(dir.z, dir.x);
		//Convert the result into degress as above is in radians
		angle = XMConvertToDegrees(angle);
		m_camOrientation.y = angle;
	}

	if (selectedObjects.empty() == false)
	{
		//We want to show all objects that were selected
		//We need to put the objects in acessending order using bubble sort
		std::vector<DisplayObject> orderedObjects = selectedObjects;
		bool shouldOrder = true;
		bool reordered = false;
		while (shouldOrder == true)
		{
			reordered = false;
			for (unsigned int i = 0; i < selectedObjects.size(); i++)
			{
				//Have to make sure I isn't greater than the size of the vector
				if (i < selectedObjects.size() - 1)
				{
					if (orderedObjects[i].m_position.x > orderedObjects[i + 1].m_position.x)
					{
						DisplayObject temp = orderedObjects[i];
						orderedObjects[i] = orderedObjects[i + 1];
						orderedObjects[i + 1] = temp;
						reordered = true;
					}
				}
			}
			if (reordered == false)
			{
				shouldOrder = false;
			}
		}
		//Now they have been get distance between max and min
		//Calculate angle between the camera and the object
		//Create a direction vector
		Vector3 midPoint = (orderedObjects.back().m_position + orderedObjects.front().m_position)/2;
		//If you are multi selecting you will probably be in mid air so don't change this
		midPoint.y = m_camPosition.y;
		m_camPosition = midPoint + Vector3(-10, 0, 10);

		Vector3 dir = midPoint - m_camPosition;
		//Get arctan of the vector
		float angle = atan2f(dir.z, dir.x);
		//Convert the result into degress as above is in radians
		angle = XMConvertToDegrees(angle);
		m_camOrientation.y = angle;

	}
	else
	{
		return;
	}
}

void Game::CreateNewObject(std::string texturespath, std::string modelspath)
{
	if (m_displayList.empty() == true)
	{
		return;
	}
	CreateCommand* createCommand = new CreateCommand;
	//This will need to deal with deletion of multiple deleted object
	//createCommand->performAction(m_displayList,texturespaths.at(0),"database/data/placeholder.cmo",m_deviceResources,*m_fxFactory);
	createCommand->performAction(m_displayList, texturespath,modelspath,m_deviceResources,*m_fxFactory);
	Commands* command = createCommand;
	commandList.push_back(command);
	//BuildDisplayList2(texturespaths.at(0), "database/data/placeholder.cmo");
}

std::vector<SceneObject> Game::getDisplayList()
{
	//NOTE: If we delete an object then undo it will be put at the bottom of the list
	// 	   This means ID 1 could be in space 15. Will this be an issue?
	// 	   Probably not since we don't care about the order and use the ID everywhere (or should!)
	// 	   If something goes wrong check here first tho

	//This needs to break the display list down into a scene object vector

	std::vector<SceneObject> objects;

	for (unsigned int i = 0; i < m_displayList.size(); i++)
	{
		SceneObject newSceneObject;
		newSceneObject.ID = m_displayList[i].m_ID;
		newSceneObject.chunk_ID = 0;
		//We need to store the model path of each object
		newSceneObject.model_path = m_displayList[i].m_modelPath;
		//And the texture path
		newSceneObject.tex_diffuse_path = m_displayList[i].m_texturePath;
		newSceneObject.posX = m_displayList[i].m_position.x;
		newSceneObject.posY = m_displayList[i].m_position.y;
		newSceneObject.posZ = m_displayList[i].m_position.z;
		newSceneObject.rotX = m_displayList[i].m_orientation.x;
		newSceneObject.rotY = m_displayList[i].m_orientation.y;
		newSceneObject.rotZ = m_displayList[i].m_orientation.z;
		newSceneObject.scaX = m_displayList[i].m_scale.x;
		newSceneObject.scaY = m_displayList[i].m_scale.y;
		newSceneObject.scaZ = m_displayList[i].m_scale.z;
		newSceneObject.render = false;
		newSceneObject.collision = 0;
		newSceneObject.collision_mesh = "";
		newSceneObject.collectable = false;
		newSceneObject.destructable = false;
		newSceneObject.health_amount = 0;
		newSceneObject.editor_render = m_displayList[i].m_render;
		newSceneObject.editor_texture_vis = true;
		newSceneObject.editor_normals_vis = false;
		newSceneObject.editor_collision_vis = false;
		newSceneObject.editor_pivot_vis = false;
		newSceneObject.pivotX = 0;
		newSceneObject.pivotY = 0;
		newSceneObject.pivotZ = 0;
		newSceneObject.snapToGround = false;
		newSceneObject.AINode = false;
		newSceneObject.audio_path = "";
		newSceneObject.volume = 0;
		newSceneObject.pitch = 0;
		newSceneObject.pan = 0;
		newSceneObject.one_shot = false;
		newSceneObject.play_on_init = false;
		newSceneObject.play_in_editor = false;
		newSceneObject.min_dist = 0;
		newSceneObject.max_dist = 0;
		newSceneObject.camera = false;
		newSceneObject.path_node = false;
		newSceneObject.path_node_start = false;
		newSceneObject.path_node_end = false;
		newSceneObject.parent_id = 0;
		//While there is an argument for actually saving the data in reality I don't see why you would ever save this as true
		newSceneObject.editor_wireframe = false;
		newSceneObject.name = "Name";

		newSceneObject.light_type = m_displayList[i].m_light_type;
		newSceneObject.light_diffuse_r = m_displayList[i].m_light_diffuse_r;
		newSceneObject.light_diffuse_g = m_displayList[i].m_light_diffuse_g;
		newSceneObject.light_diffuse_b = m_displayList[i].m_light_diffuse_b;
		newSceneObject.light_specular_r = m_displayList[i].m_light_specular_r;
		newSceneObject.light_specular_g = m_displayList[i].m_light_specular_g;
		newSceneObject.light_specular_b = m_displayList[i].m_light_specular_b;
		newSceneObject.light_spot_cutoff = m_displayList[i].m_light_spot_cutoff;
		newSceneObject.light_constant = m_displayList[i].m_light_constant;
		newSceneObject.light_linear = m_displayList[i].m_light_linear;
		newSceneObject.light_quadratic = m_displayList[i].m_light_quadratic;


		//send completed object to scenegraph
		objects.push_back(newSceneObject);
	}



	return objects;
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif


#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);

    m_fxFactory = std::make_unique<EffectFactory>(device);
	m_fxFactory->SetDirectory(L"database/data/"); //fx Factory will look in the database directory
	m_fxFactory->SetSharing(false);	//we must set this to false otherwise it will share effects based on the initial tex loaded (When the model loads) rather than what we will change them to.

    m_sprites = std::make_unique<SpriteBatch>(context);

    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

    m_batchEffect = std::make_unique<BasicEffect>(device);
    m_batchEffect->SetVertexColorEnabled(true);

    {
        void const* shaderByteCode;
        size_t byteCodeLength;

        m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

        DX::ThrowIfFailed(
            device->CreateInputLayout(VertexPositionColor::InputElements,
                VertexPositionColor::InputElementCount,
                shaderByteCode, byteCodeLength,
                m_batchInputLayout.ReleaseAndGetAddressOf())
        );
    }

    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");

//    m_shape = GeometricPrimitive::CreateTeapot(context, 4.f, 8);

    // SDKMESH has to use clockwise winding with right-handed coordinates, so textures are flipped in U
    m_model = Model::CreateFromSDKMESH(device, L"tiny.sdkmesh", *m_fxFactory);
	

    // Load textures
    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(device, L"seafloor.dds", nullptr, m_texture1.ReleaseAndGetAddressOf())
    );

    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(device, L"windowslogo.dds", nullptr, m_texture2.ReleaseAndGetAddressOf())
    );

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        1000.0f
    );

    m_batchEffect->SetProjection(m_projection);
	
}

void Game::undoAction()
{
	if (commandList.size() <= 0)
	{
		return;
	}

	Commands* commandToUndo = commandList.front();
	commandList.pop_front();
	if (commandToUndo->getType() == Commands::CommandType::Create)
	{
		DeleteCommand* deleteCommand = new DeleteCommand;
		//This actually needs to be the ID of the object created
		if (commandToUndo->getCreatedObjects().empty() == false)
		{
			deleteCommand->performAction(m_displayList, commandToUndo->getCreatedObjects());
		}
		else
		{
			deleteCommand->performAction(m_displayList, commandToUndo->getCreatedObject().m_ID);
		}
		//Maybe change this, just means after an action there are no selected objects
		selectedObjects.clear();
		Commands* command = deleteCommand;
		UndonecommandList.push_back(command);
	}
	else if (commandToUndo->getType() == Commands::CommandType::Delete)
	{
		CreateCommand* createCommand = new CreateCommand;
		//This will need to deal with deletion of multiple deleted object
		if (commandToUndo->getDeletedObjects().size() > 0)
		{
			createCommand->performAction(m_displayList, commandToUndo->getDeletedObjects(), m_fxFactory,false,true);
		}
		else
		{
			createCommand->performAction(m_displayList,commandToUndo->getDeletedObject(), m_fxFactory,false,true);
		}
		Commands* command = createCommand;
		UndonecommandList.push_back(command);
	}

}

void Game::RedoAction()
{
	if (UndonecommandList.size() <= 0)
	{
		return;
	}

	Commands* commandToDo = UndonecommandList.front();
	UndonecommandList.pop_front();
	if (commandToDo->getType() == Commands::CommandType::Create)
	{
		DeleteCommand* deleteCommand = new DeleteCommand;
		//This actually needs to be the ID of the object created
		if (commandToDo->getCreatedObjects().empty() == false)
		{
			deleteCommand->performAction(m_displayList, commandToDo->getCreatedObjects());
		}
		else
		{
			deleteCommand->performAction(m_displayList, commandToDo->getCreatedObject().m_ID);
		}
		//Maybe change this, just means after an action there are no selected objects
		selectedObjects.clear();
		Commands* command = deleteCommand;
		commandList.push_back(command);
	}
	else if (commandToDo->getType() == Commands::CommandType::Delete)
	{
		CreateCommand* createCommand = new CreateCommand;
		//This will need to deal with deletion of multiple deleted object
		if (commandToDo->getDeletedObjects().size() > 0)
		{
			createCommand->performAction(m_displayList, commandToDo->getDeletedObjects(), m_fxFactory);
		}
		else
		{
			createCommand->performAction(m_displayList, commandToDo->getDeletedObject(), m_fxFactory);
		}
		Commands* command = createCommand;
		UndonecommandList.push_back(command);
	}
}

void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_batch.reset();
    m_batchEffect.reset();
    m_font.reset();
    m_shape.reset();
    m_model.reset();
    m_texture1.Reset();
    m_texture2.Reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion

std::wstring StringToWCHART(std::string s)
{

	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}
