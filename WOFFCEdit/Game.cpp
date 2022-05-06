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

	m_camera.setup();
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
	//camera motion is on a plane, so kill the 7 component of the look direction
	Vector3 planarMotionVector = m_camera.getCameraLookDirection();
	planarMotionVector.y = 0.0;

	if (m_InputCommands.rotRight)
	{
		m_camera.rotateRight();
	}
	if (m_InputCommands.rotLeft)
	{
		m_camera.rotateLeft();
	}
	if (m_InputCommands.rotUp)
	{
		m_camera.rotateUp();
	}
	if (m_InputCommands.rotDown)
	{
		m_camera.rotateDown();
	}

	//process input and update stuff
	if (m_InputCommands.forward)
	{	
		m_camera.moveForward();
	}

	if (m_InputCommands.back)
	{
		m_camera.moveBack();
	}

	if (m_InputCommands.right && m_InputCommands.alt)
	{
		archballR();
	}
	else if(m_InputCommands.right)
	{
		m_camera.moveRight();
	}

	if (m_InputCommands.left && m_InputCommands.alt)
	{
		archballL();
	}
	else if(m_InputCommands.left)
	{
		m_camera.moveLeft();
	}


	if (m_InputCommands.deleteObject && inputDown == false)
	{
		inputDown = true;
		if (selectedObjects.empty() == false)
		{
			DeleteCommand* deleteCommand =  new DeleteCommand;
			deleteCommand->performAction(m_displayList, selectedObjects);
			selectedObjects.clear();
			Commands* command = deleteCommand;
			commandList.push(command);
		}
		if (selectedObject != NULL)
		{
			DeleteCommand* deleteCommand = new DeleteCommand;
			cleanupAllArrows();
			deleteCommand->performAction(m_displayList, selectedObject->m_ID);
			selectedObject = NULL;
			Commands* command = deleteCommand;
			commandList.push(command);
		}
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

	m_camera.update(shouldResetOrientation);
	if (shouldResetOrientation == true)
	{
		shouldResetOrientation = false;
	}

	//apply camera vectors
    //m_view = Matrix::CreateLookAt(m_camera.getCameraPosition(), m_camera.getCameraLookAt(), Vector3::UnitY);
	m_view = m_camera.getViewMatrix();;

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
	std::wstring var = L"Cam X: " + std::to_wstring(m_camera.getCameraPosition().x) + L"Cam Z: " + std::to_wstring(m_camera.getCameraPosition().z);
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

	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(),0);
	context->RSSetState(m_states->CullNone());
	//context->RSSetState(m_states->Wireframe());		//uncomment for wireframe

	//Render the batch,  This is handled in the Display chunk becuase it has the potential to get complex
	//RENDER TERRAIN - To fix the issue of the terrain texture dissaperaing, just hid a box in the world? Player never sees it so can't delete it! (Thanks Matt!)
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

#pragma endregion

void Game::setSelectedObject(DisplayObject* newObject)
{
	if (selectedObject != NULL)
	{
		selectedObject->m_wireframe = false;
		selectedObject->m_selected = false;
		//Clear all arrows
		cleanupAllArrows();
	}
	selectedObject = newObject;
	selectedObject->m_wireframe = true;
	selectedObject->m_selected = true;

	//Now we need to spawn the selection arrows around the object in 6 places
	for (unsigned int i = 0; i < 6; i++)
	{
		DragArrow newArrow;
		switch (i)
		{
		case 0:
			//Up
			newArrow.setup(newObject->m_position.x, newObject->m_position.y + 1, newObject->m_position.z, 0, 0, 90, m_deviceResources, *m_fxFactory);
			newArrow.up = true;
			newArrow.attachedObject = newObject;
			break;
		case 1:
			//Down (should also rotate this to face down once actual model is in)
			newArrow.setup(newObject->m_position.x, newObject->m_position.y - 1, newObject->m_position.z, 0, 0, -90, m_deviceResources, *m_fxFactory, m_dragArrowList[0].m_model, m_dragArrowList[0].m_texture_diffuse);
			newArrow.down = true;
			newArrow.attachedObject = newObject;
			break;
		case 2:
			//Left 
			newArrow.setup(newObject->m_position.x - 1, newObject->m_position.y, newObject->m_position.z, 0, -180, 0, m_deviceResources, *m_fxFactory, m_dragArrowList[0].m_model, m_dragArrowList[0].m_texture_diffuse);
			newArrow.left = true;
			newArrow.attachedObject = newObject;
			break;
		case 3:
			//Right
			newArrow.setup(newObject->m_position.x + 1, newObject->m_position.y, newObject->m_position.z, 0, 0, 0, m_deviceResources, *m_fxFactory, m_dragArrowList[0].m_model, m_dragArrowList[0].m_texture_diffuse);
			newArrow.right = true;
			newArrow.attachedObject = newObject;
			break;
		case 4:
			//back
			newArrow.setup(newObject->m_position.x, newObject->m_position.y, newObject->m_position.z - 1, 0, 90, 0, m_deviceResources, *m_fxFactory, m_dragArrowList[0].m_model, m_dragArrowList[0].m_texture_diffuse);
			newArrow.back = true;
			newArrow.attachedObject = newObject;
			break;
		case 5:
			//forward
			newArrow.setup(newObject->m_position.x, newObject->m_position.y, newObject->m_position.z + 1, 0, -90, 0, m_deviceResources, *m_fxFactory, m_dragArrowList[0].m_model, m_dragArrowList[0].m_texture_diffuse);
			newArrow.forward = true;
			newArrow.attachedObject = newObject;
			break;
		default:
			break;
		}
		m_dragArrowList.push_back(newArrow);
	}
}
void Game::setSelectedObjects(std::vector<DisplayObject*> newObjects)
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
				if (selectedObjects[i]->m_ID == m_displayList[j].m_ID)
				{
					m_displayList[j].m_wireframe = false;
					m_displayList[j].m_selected = false;
				}
			}
		}
		selectedObjects.clear();
		cleanupAllArrows();
	}
	//We need to do this in a better way
		for (int i = 0; i < newObjects.size(); i++)
		{
			newObjects[i]->m_wireframe = true;
			newObjects[i]->m_selected = true;
			selectedObjects.push_back(newObjects[i]);
			//Now we need to spawn the selection arrows around the object in 6 places
			for (unsigned int i = 0; i < 6; i++)
			{
				DragArrow newArrow;
				switch (i)
				{
				case 0:
					//Up
					newArrow.setup(newObjects[i]->m_position.x, newObjects[i]->m_position.y + 1, newObjects[i]->m_position.z, 0, 0, 0, m_deviceResources, *m_fxFactory);
					newArrow.attachedObject = newObjects[i];
					newArrow.up = true;
					break;
				case 1:
					//Down (should also rotate this to face down once actual model is in)
					newArrow.setup(newObjects[i]->m_position.x, newObjects[i]->m_position.y - 1, newObjects[i]->m_position.z, 0, 0, 0, m_deviceResources, *m_fxFactory);
					newArrow.attachedObject = newObjects[i];
					newArrow.down = true;
					break;
				case 2:
					//Left 
					newArrow.setup(newObjects[i]->m_position.x - 1, newObjects[i]->m_position.y, newObjects[i]->m_position.z, 0, 0, 90, m_deviceResources, *m_fxFactory);
					newArrow.attachedObject = newObjects[i];
					newArrow.left = true;
					break;
				case 3:
					//Right
					newArrow.setup(newObjects[i]->m_position.x + 1, newObjects[i]->m_position.y, newObjects[i]->m_position.z, 0, 0, -90, m_deviceResources, *m_fxFactory);
					newArrow.attachedObject = newObjects[i];
					newArrow.right = true;
					break;
				case 4:
					//back
					newArrow.setup(newObjects[i]->m_position.x, newObjects[i]->m_position.y, newObjects[i]->m_position.z - 1, -90, 0, 0, m_deviceResources, *m_fxFactory);
					newArrow.attachedObject = newObjects[i];
					newArrow.back = true;
					break;
				case 5:
					//forward
					newArrow.setup(newObjects[i]->m_position.x, newObjects[i]->m_position.y, newObjects[i]->m_position.z + 1, 90, 0, 0, m_deviceResources, *m_fxFactory);
					newArrow.attachedObject = newObjects[i];
					newArrow.forward = true;
					break;
				default:
					break;
				}
				m_dragArrowList.push_back(newArrow);
			}
		}
}

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
				//m_displayList[i].m_wireframe = true;

				if (m_InputCommands.multipick && m_displayList[i].m_selected == false)
				{
					m_displayList[i].m_selected = true;
					if (selectedObject != NULL)
					{
						//Since we are adding in click and drag we should move this into a function so we can add the arrows easily
						//selectedObjects.push_back(*selectedObject);
						pushBackNewSelectedObject(selectedObject);
					}
					pushBackNewSelectedObject(&m_displayList[i]);
					//selectedObjects.push_back(m_displayList[i]);
				}
				else if (m_InputCommands.multipick && m_displayList[i].m_selected == true)
				{
					m_displayList[i].m_selected = false;
					eraseSelectedObject(&m_displayList[i]);
					//Got to find object in selected objects list
					//for (int j = 0; j < selectedObjects.size(); j++)
					//{
					//	if (m_displayList[i].m_ID == selectedObjects.at(j).m_ID)
					//	{
					//		//Found object, remove, again function this so we can remove arrows
					//		selectedObjects.erase(selectedObjects.begin() + j);
					//		break;
					//	}
					//}
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

void Game::clickAndDrag()
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

	bool mouseMovedY = false;
	bool mouseMovedX = false;

	//Test for collision against a drag arrow
	for (int i = 0; i < m_dragArrowList.size(); i++)
	{
		//Get the scale factor and translation of the object
		const XMVECTORF32 scale = { m_dragArrowList[i].m_scale.x,
			m_dragArrowList[i].m_scale.y, m_dragArrowList[i].m_scale.z };
		const XMVECTORF32 translate = { m_dragArrowList[i].m_position.x,
			m_dragArrowList[i].m_position.y, m_dragArrowList[i].m_position.z };
		//Convert eular angles into a quaterion for the rotation of the object
		XMVECTOR rotate = Quaternion::CreateFromYawPitchRoll(m_dragArrowList[i].m_orientation.y * 3.1415 / 180,
			m_dragArrowList[i].m_orientation.x * 3.1415 / 180,
			m_dragArrowList[i].m_orientation.z * 3.1415 / 180);
		//Create set the matrix of the selected object in the world based on the translation,scale and roation
		XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);
		//Unproject the points on the near and far plane, with respect to the matrix we just created
		XMVECTOR nearPoint = XMVector3Unproject(nearSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);
		XMVECTOR farPoint = XMVector3Unproject(farSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);

		//Turn the transformed points into our picking vector
		XMVECTOR pickingVector = farPoint - nearPoint;
		pickingVector = XMVector3Normalize(pickingVector);

		//We want to detect if the user is clicking the selected arrow
		//This is almost really good. One last big issue, the object moves further than the mouse does
		//This means the mouse ends up away from the object so the user has to keep up with it
		//It should move at the same rate as the mouse to make it look like it's attached
		for (unsigned int y = 0; y < m_dragArrowList[i].m_model.get()->meshes.size(); y++)
		{
			if (m_dragArrowList[i].m_model.get()->meshes[y]->boundingBox.Intersects(nearPoint, pickingVector, pickedDistance) && pickedDistance < closestDistance)
			{
				if (selectedArrow == NULL)
				{
					return;
				}
				float newMouseY = m_InputCommands.mouse_Y;
				float newMouseX = m_InputCommands.mouse_X;
				if (newMouseX != mouseXOnClick)
				{
					mouseMovedX = true;
				}
				if (newMouseY != mouseYOnClick)
				{
					mouseMovedY = true;
				}

				//Did the mouse move?
				if (mouseMovedX == false && mouseMovedY == false)
				{
					break;
				}

				//This is just to check if it's selected, can change later
				if (selectedObjects.empty() == true)
				{
					if (m_dragArrowList[i].attachedObject == selectedArrow->attachedObject)
					{
						//Now we need to move based on what direction that arrow is
						if (selectedArrow->back == true)
						{
							selectedObject->m_position.z = selectedObject->m_position.z - 0.1f;
						}
						if (selectedArrow->left == true)
						{
							selectedObject->m_position.x = selectedObject->m_position.x - 0.1f;
						}
						if (selectedArrow->right == true)
						{
							selectedObject->m_position.x = selectedObject->m_position.x + 0.1f;
						}
						if (selectedArrow->forward == true)
						{
							//Calculate the distance between where the mouse is and where the object is
							selectedObject->m_position.z = selectedObject->m_position.z + 0.1f;
						}
						if (selectedArrow->up == true)
						{
							selectedObject->m_position.y = selectedObject->m_position.y + 0.1f;
						}
						if (selectedArrow->down == true)
						{
							selectedObject->m_position.y = selectedObject->m_position.y - 0.1f;
						}
					}

				}
				else
				{
					//Multi
											
					//Now we need to move based on what direction that arrow is
					if (selectedArrow->back == true)
					{
						for (unsigned int k = 0; k < selectedObjects.size(); k++)
						{
							selectedObjects[k]->m_position.z = selectedObjects[k]->m_position.z - 0.1f;
						}
					}
					if (selectedArrow->left == true)
					{
						for (unsigned int k = 0; k < selectedObjects.size(); k++)
						{
							selectedObjects[k]->m_position.x = selectedObjects[k]->m_position.x - 0.1f;
						}
					}
					if (selectedArrow->right == true)
					{
						for (unsigned int k = 0; k < selectedObjects.size(); k++)
						{
							selectedObjects[k]->m_position.x = selectedObjects[k]->m_position.x + 0.1f;
						}
					}
					if (selectedArrow->forward == true)
					{
						for (unsigned int k = 0; k < selectedObjects.size(); k++)
						{
							selectedObjects[k]->m_position.z = selectedObjects[k]->m_position.z + 0.1f;
						}
					}
					if (selectedArrow->up == true)
					{
						for (unsigned int k = 0; k < selectedObjects.size(); k++)
						{
							selectedObjects[k]->m_position.y = selectedObjects[k]->m_position.y + 0.1f;
						}
					}
					if (selectedArrow->down == true)
					{
						for (unsigned int k = 0; k < selectedObjects.size(); k++)
						{
							selectedObjects[k]->m_position.y = selectedObjects[k]->m_position.y - 0.1f;
						}
					}
				}
				updateAllArrowpositions();
				m_DragStarted = true;
						//for (unsigned int j = 0; j < m_dragArrowList.size(); j++)
						//{
						//	m_dragArrowList[j].updateDragArrow();
						//}
						//Old mouse code (try to get to work)
						//Calculate the distance between where the mouse is and where the object is
						//Get mouse Y (remember it's in screenspace so convert to world space)
						//XMVECTOR mousePositionAsVector = XMVectorSet(m_InputCommands.mouse_X, m_InputCommands.mouse_Y, 0, 0);
						//The view matrix might be the issue here? It's the only thing I can think of that would be changed by the camera moving
						//XMVECTOR mousePoint = XMVector3Unproject(mousePositionAsVector, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, m_world);
						//Scale this based on the distance between the camera and the object
						//float YDistance = mousePoint.m128_f32[1] - selectedObject->m_position.y;
						//selectedObject->m_position.y = selectedObject->m_position.y + YDistance;
			}
		}
	}
}

void Game::checkForDragArrow()
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

	//Test for collision against a drag arrow
	for (int i = 0; i < m_dragArrowList.size(); i++)
	{
		//Get the scale factor and translation of the object
		const XMVECTORF32 scale = { m_dragArrowList[i].m_scale.x,
			m_dragArrowList[i].m_scale.y, m_dragArrowList[i].m_scale.z };
		const XMVECTORF32 translate = { m_dragArrowList[i].m_position.x,
			m_dragArrowList[i].m_position.y, m_dragArrowList[i].m_position.z };
		//Convert eular angles into a quaterion for the rotation of the object
		XMVECTOR rotate = Quaternion::CreateFromYawPitchRoll(m_dragArrowList[i].m_orientation.y * 3.1415 / 180,
			m_dragArrowList[i].m_orientation.x * 3.1415 / 180,
			m_dragArrowList[i].m_orientation.z * 3.1415 / 180);
		//Create set the matrix of the selected object in the world based on the translation,scale and roation
		XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);
		//Unproject the points on the near and far plane, with respect to the matrix we just created
		XMVECTOR nearPoint = XMVector3Unproject(nearSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);
		XMVECTOR farPoint = XMVector3Unproject(farSource, 0.0f, 0.0f, m_ScreenDimensions.right, m_ScreenDimensions.bottom, m_deviceResources->GetScreenViewport().MinDepth, m_deviceResources->GetScreenViewport().MaxDepth, m_projection, m_view, local);

		//Turn the transformed points into our picking vector
		XMVECTOR pickingVector = farPoint - nearPoint;
		pickingVector = XMVector3Normalize(pickingVector);

		bool hitModel = false;
		//When we have selected an arrow we don't want to select any others.
		for (unsigned int y = 0; y < m_dragArrowList[i].m_model.get()->meshes.size(); y++)
		{
			if (m_dragArrowList[i].m_model.get()->meshes[y]->boundingBox.Intersects(nearPoint, pickingVector, pickedDistance) && pickedDistance < closestDistance && hitModel == false)
			{
				selectedArrow = &m_dragArrowList[i];
				selectedArrow->m_wireframe = true;
				closestDistance = pickedDistance;
			}
		}
	}
	mouseYOnClick = m_InputCommands.mouse_Y;
	mouseXOnClick = m_InputCommands.mouse_X;
	objectIDs.clear();
	objectsOriginalPositionsX.clear();
	objectsOriginalPositionsY.clear();
	objectsOriginalPositionsZ.clear();
	if (selectedObjects.empty() == true)
	{
		if (selectedObject != NULL)
		{
			objectsOriginalPositionX = selectedObject->m_position.x;
			objectsOriginalPositionY = selectedObject->m_position.y;
			objectsOriginalPositionZ = selectedObject->m_position.z;
		}
		else
		{
			return;
		}
	}
	else
	{
		for (unsigned int k = 0; k < selectedObjects.size(); k++)
		{
			objectsOriginalPositionsX.push_back(selectedObjects[k]->m_position.x);
			objectsOriginalPositionsY.push_back(selectedObjects[k]->m_position.y);
			objectsOriginalPositionsZ.push_back(selectedObjects[k]->m_position.z);
			objectIDs.push_back(selectedObjects[k]->m_ID);
		}
	}
}
void Game::dragFinished()
{
	//BUG!: This is called even when an MFC window is open, this floods the command list with bad moves
	//To fix: Try to detect when MFC is open and ignore these clicks

	if (m_DragStarted != true)
	{
		return;
	}
	else
	{
		m_DragStarted = false;
	}

	if (selectedObjects.empty() == true)
	{
		if (selectedObject != NULL)
		{
			UndoMove* UndoMoveCommand = new UndoMove;
			UndoMoveCommand->setup(selectedObject->m_ID, XMVECTOR{ objectsOriginalPositionX,objectsOriginalPositionY,objectsOriginalPositionZ });
			Commands* command = UndoMoveCommand;
			commandList.push(command);
		}
		else
		{
			return;
		}
	}
	else
	{
		UndoMove* UndoMoveCommand = new UndoMove;
		//Make an XMVECTOR to pass in (again this is do solve a strange compilation error)
		std::vector<XMVECTOR> objectOriginalPositions;
		for (unsigned int i = 0; i < objectsOriginalPositionsX.size(); i++)
		{
			objectOriginalPositions.push_back(XMVECTOR{ objectsOriginalPositionsX[i],objectsOriginalPositionsY[i],objectsOriginalPositionsZ[i] });
		}
		UndoMoveCommand->setup(objectIDs, objectOriginalPositions);
		Commands* command = UndoMoveCommand;
		commandList.push(command);
	}
}
void Game::setID(int newID)
{
	ID = newID;
}

void Game::copyObject()
{
	if (selectedObjects.empty() == false)
	{
		copyCommand.performAction(&selectedObjects);
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
	commandList.push(command);
}

void Game::focusOnItem()
{
	if (selectedObject == NULL)
	{
	}
	else
	{
		//Move camera to object position
		shouldResetOrientation = true;
		m_camera.setCameraPosition(selectedObject->m_position + Vector3(0, 1, 7));

		//Calculate angle between the camera and the object
		//Create a direction vector
		Vector3 dir = selectedObject->m_position - m_camera.getCameraPosition();
		//Get arctan of the vector
		float angle = atan2f(dir.z, dir.x);
		//Convert the result into degress as above is in radians
		angle = XMConvertToDegrees(angle);
		m_camera.setCameraOrientation(Vector3(m_camera.getCameraOrientation().x, angle, m_camera.getCameraOrientation().z));
	}

	if (selectedObjects.empty() == false)
	{
		//We want to show all objects that were selected
		//We need to put the objects in acessending order using bubble sort
		std::vector<DisplayObject*> orderedObjects = selectedObjects;
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
					if (orderedObjects[i]->m_position.x > orderedObjects[i + 1]->m_position.x)
					{
						DisplayObject* temp = orderedObjects[i];
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
		Vector3 midPoint = (orderedObjects.back()->m_position + orderedObjects.front()->m_position)/2;
		//If you are multi selecting you will probably be in mid air so don't change this
		midPoint.y = m_camera.getCameraPosition().y;
		m_camera.setCameraPosition(midPoint + Vector3(0, 0, midPoint.x));

		Vector3 dir = midPoint - m_camera.getCameraPosition();
		//Get arctan of the vector
		float angle = atan2f(dir.z, dir.x);
		//Convert the result into degress as above is in radians
		angle = XMConvertToDegrees(angle);
		m_camera.setCameraOrientation(Vector3(m_camera.getCameraOrientation().x, angle, m_camera.getCameraOrientation().z));
		//m_camOrientation.y = angle;

	}
	else
	{
		//User has no objects selected
		return;
	}
}

void Game::archballL()
{
	//If the user has multiple object selected, don't allow rotation
	if (selectedObjects.empty() == false)
	{
		return;
	}

	//User has no selected object
	if (selectedObject == NULL)
	{
		return;
	}

	//Make sure the user is looking at the camera
	Vector3 dir = selectedObject->m_position - m_camera.getCameraPosition();
	//Get arctan of the vector
	float angle = atan2f(dir.z, dir.x);
	//Convert the result into degress as above is in radians
	angle = XMConvertToDegrees(angle);
	m_camera.setCameraOrientation(Vector3(m_camera.getCameraOrientation().x, angle, m_camera.getCameraOrientation().z));

	m_camera.orbitLeft();
}

void Game::archballR()
{
	//If the user has multiple object selected, don't allow rotation
	if (selectedObjects.empty() == false)
	{
		return;
	}

	//User has no selected object
	if (selectedObject == NULL)
	{
		return;
	}

	//Make sure the user is looking at the camera
	Vector3 dir = selectedObject->m_position - m_camera.getCameraPosition();
	//Get arctan of the vector
	float angle = atan2f(dir.z, dir.x);
	//Convert the result into degress as above is in radians
	angle = XMConvertToDegrees(angle);
	m_camera.setCameraOrientation(Vector3(m_camera.getCameraOrientation().x, angle, m_camera.getCameraOrientation().z));

	m_camera.orbitRight();
}

void Game::CreateNewObject(std::string texturespath, std::string modelspath, DirectX::XMVECTOR position, DirectX::XMVECTOR scale, DirectX::XMVECTOR rotation)
{
	if (m_displayList.empty() == true)
	{
		return;
	}
	CreateCommand* createCommand = new CreateCommand;
	createCommand->performAction(m_displayList, texturespath,modelspath,position,scale,rotation, m_deviceResources,*m_fxFactory);
	Commands* command = createCommand;
	commandList.push(command);
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

void Game::pushBackNewSelectedObject(DisplayObject* newObject)
{
	//check to see if the object already exsist in the vector
	for (unsigned int i = 0; i < selectedObjects.size(); i++)
	{
		if (newObject->m_ID == selectedObjects[i]->m_ID)
		{
			//Object is a duplicate, ignore
			return;
		}
	}
	newObject->m_selected = true;
	newObject->m_wireframe = true;
	selectedObjects.push_back(newObject);
	//Now we need to spawn the selection arrows around the object in 6 places
	for (unsigned int i = 0; i < 6; i++)
	{
		DragArrow newArrow;
		switch (i)
		{
		case 0:
			//Up
			newArrow.setup(newObject->m_position.x, newObject->m_position.y + 1, newObject->m_position.z, 0, 0, 90, m_deviceResources, *m_fxFactory);
			newArrow.up = true;
			newArrow.attachedObject = newObject;
			break;
		case 1:
			//Down
			newArrow.setup(newObject->m_position.x, newObject->m_position.y - 1, newObject->m_position.z, 0, 0, -90, m_deviceResources, *m_fxFactory, m_dragArrowList[0].m_model, m_dragArrowList[0].m_texture_diffuse);
			newArrow.down = true;
			newArrow.attachedObject = newObject;
			break;
		case 2:
			//Left 
			newArrow.setup(newObject->m_position.x - 1, newObject->m_position.y, newObject->m_position.z, 0, -180, 0, m_deviceResources, *m_fxFactory, m_dragArrowList[0].m_model, m_dragArrowList[0].m_texture_diffuse);
			newArrow.left = true;
			newArrow.attachedObject = newObject;
			break;
		case 3:
			//Right
			newArrow.setup(newObject->m_position.x + 1, newObject->m_position.y, newObject->m_position.z, 0, 0, 0, m_deviceResources, *m_fxFactory, m_dragArrowList[0].m_model, m_dragArrowList[0].m_texture_diffuse);
			newArrow.right = true;
			newArrow.attachedObject = newObject;
			break;
		case 4:
			//back
			newArrow.setup(newObject->m_position.x, newObject->m_position.y, newObject->m_position.z - 1, 0, 90, 0, m_deviceResources, *m_fxFactory, m_dragArrowList[0].m_model, m_dragArrowList[0].m_texture_diffuse);
			newArrow.back = true;
			newArrow.attachedObject = newObject;
			break;
		case 5:
			//forward
			newArrow.setup(newObject->m_position.x, newObject->m_position.y, newObject->m_position.z + 1, 0, -90, 0, m_deviceResources, *m_fxFactory, m_dragArrowList[0].m_model, m_dragArrowList[0].m_texture_diffuse);
			newArrow.forward = true;
			newArrow.attachedObject = newObject;
			break;
		default:
			break;
		}
		m_dragArrowList.push_back(newArrow);
	}
}

void Game::eraseSelectedObject(DisplayObject* newObject)
{
	//Find the object in the selected object vector
	for (int j = 0; j < selectedObjects.size(); j++)
	{
		if (newObject->m_ID == selectedObjects.at(j)->m_ID)
		{
			//Found object, remove, again function this so we can remove arrows
			selectedObjects.erase(selectedObjects.begin() + j);
		}
	}
	cleanupAllArrows();
}

void Game::cleanupAllArrows()
{
	//Now we need to clear the arrows around that object
	for (int i = 0; i < m_dragArrowList.size(); i++)
	{
		m_dragArrowList[i].cleanUp();
	}
	m_dragArrowList.clear();
}

void Game::updateAllArrowpositions()
{
	for (unsigned int j = 0; j < m_dragArrowList.size(); j++)
	{
		m_dragArrowList[j].updateDragArrow();
	}
}

void Game::mouseCameraMovement()
{
	static float oldMouseX = 0.0f;
	static float oldMouseY = 0.0f;

	float newMouseX = m_InputCommands.mouse_X;
	float newMouseY = m_InputCommands.mouse_Y;

	if (newMouseX > oldMouseX)
	{
		//Mouse moved further left, rotate right
		m_camera.rotateLeft();
		oldMouseX = newMouseX;
	}

	if (newMouseX < oldMouseX)
	{
		//Mouse moved further right
		m_camera.rotateRight();
		oldMouseX = newMouseX;
	}

	if (newMouseY > oldMouseY)
	{
		//Mouse moved further up
		m_camera.rotateUp();
		oldMouseY = newMouseY;
		return;
	}

	if (newMouseY < oldMouseY)
	{
		//Move moved further down
		m_camera.rotateDown();
		oldMouseY = newMouseY;
		return;
	}
}

DisplayObject* Game::getSelectedObject()
{
	return selectedObject;
}

std::vector<DisplayObject*> Game::getSelectedObjects()
{
	return selectedObjects;
}

std::shared_ptr<DX::DeviceResources> Game::getD3DDevices()
{
	return m_deviceResources;
}

DirectX::IEffectFactory& Game::getfxFactory()
{
	// TODO: insert return statement here
	return *m_fxFactory;
}

std::stack<Commands*>* Game::getCommandList()
{
	return &commandList;
}

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

void Game::undoAction()
{
	if (commandList.size() <= 0)
	{
		return;
	}

	Commands* commandToUndo = commandList.top();
	commandList.pop();
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
		UndonecommandList.push(command);
	}
	else if (commandToUndo->getType() == Commands::CommandType::Delete)
	{
		CreateCommand* createCommand = new CreateCommand;
		if (commandToUndo->getDeletedObjects().size() > 0)
		{
			createCommand->performAction(m_displayList, commandToUndo->getDeletedObjects(), m_fxFactory,false,true);
		}
		else
		{
			createCommand->performAction(m_displayList,commandToUndo->getDeletedObject(), m_fxFactory,false,true);
		}
		Commands* command = createCommand;
		UndonecommandList.push(command);
	}
	else if (commandToUndo->getType() == Commands::CommandType::UndoMove)
	{
		MoveCommand* moveCommand = new MoveCommand;
		if (commandToUndo->getMovedObjectsIDs().empty() == false)
		{
			//Multi
			moveCommand->setup(commandToUndo->getMovedObjectsIDs(), commandToUndo->getMovedObjectsOriginalPositions());
			moveCommand->performAction(&m_displayList);
		}
		else
		{
			moveCommand->setup(commandToUndo->getMovedObjectsID(), commandToUndo->getMovedObjectsOriginalPosition());
			moveCommand->performAction(&m_displayList);
		}
		//Update all drag arrows
		for (unsigned int j = 0; j < m_dragArrowList.size(); j++)
		{
			m_dragArrowList[j].updateDragArrow();
		}
		Commands* command = moveCommand;
		UndonecommandList.push(command);
	}
	else if (commandToUndo->getType() == Commands::CommandType::Move)
	{
		UndoMove* undoMoveCommand = new UndoMove;
		if (commandToUndo->getMovedObjectsIDs().empty() == false)
		{
			//Multi
			undoMoveCommand->setup(commandToUndo->getMovedObjectsIDs(), commandToUndo->getMovedObjectsOriginalPositions());
			undoMoveCommand->performAction(&m_displayList);
		}
		else
		{
			//Get the display object that was moved
			int IDOfMovedObject = commandToUndo->getMovedObjectsID();
			DisplayObject* movedObject;
			for (unsigned int i = 0; i < m_displayList.size(); i++)
			{
				if (IDOfMovedObject == m_displayList[i].m_ID)
				{
					movedObject = &m_displayList.at(i);
				}
			}
			undoMoveCommand->setup(IDOfMovedObject, commandToUndo->getMovedObjectsOriginalPosition());
			undoMoveCommand->performAction(&m_displayList);
		}
		//Update all drag arrows
		for (unsigned int j = 0; j < m_dragArrowList.size(); j++)
		{
			m_dragArrowList[j].updateDragArrow();
		}
		Commands* command = undoMoveCommand;
		UndonecommandList.push(command);
	}
	else if (commandToUndo->getType() == Commands::CommandType::UndoManipulation)
	{
		//Redo manipulation
		RedoManipulationCommand* redoManipulationCommand = new RedoManipulationCommand;
		if (commandToUndo->getStoredObjects().empty() == false)
		{
			//Multi
			redoManipulationCommand->setup(commandToUndo->getStoredObjects());
			redoManipulationCommand->performAction(&m_displayList);
		}
		else
		{
			redoManipulationCommand->setup(commandToUndo->getStoredObject());
			redoManipulationCommand->performAction(&m_displayList);
		}
		Commands* command = redoManipulationCommand;
		UndonecommandList.push(command);
		updateAllArrowpositions();
	}
	else if (commandToUndo->getType() == Commands::CommandType::RedoManipulation)
	{
		//Undo manipulation
		UndoManipulationCommand* undoManipulationCommand = new UndoManipulationCommand;
		if (commandToUndo->getStoredObjects().empty() == false)
		{
			//Multi
			undoManipulationCommand->setup(commandToUndo->getStoredObjects());
			undoManipulationCommand->performAction(&m_displayList);
		}
		else
		{
			//Single
			undoManipulationCommand->setup(commandToUndo->getStoredObject());
			undoManipulationCommand->performAction(&m_displayList);
		}
		Commands* command = undoManipulationCommand;
		UndonecommandList.push(command);
		updateAllArrowpositions();
	}
}

void Game::RedoAction()
{
	if (UndonecommandList.size() <= 0)
	{
		return;
	}

	Commands* commandToDo = UndonecommandList.top();
	UndonecommandList.pop();
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
		commandList.push(command);
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
		commandList.push(command);
	}
	else if (commandToDo->getType() == Commands::CommandType::UndoMove)
	{
		MoveCommand* moveCommand = new MoveCommand;
		if (commandToDo->getMovedObjectsIDs().empty() == false)
		{
			//Multi
			moveCommand->setup(commandToDo->getMovedObjectsIDs(), commandToDo->getMovedObjectsOriginalPositions());
			moveCommand->performAction(&m_displayList);
		}
		else
		{
			moveCommand->setup(commandToDo->getMovedObjectsID(), commandToDo->getMovedObjectsOriginalPosition());
			moveCommand->performAction(&m_displayList);
		}
		//Update all drag arrows
		for (unsigned int j = 0; j < m_dragArrowList.size(); j++)
		{
			m_dragArrowList[j].updateDragArrow();
		}
		Commands* command = moveCommand;
		commandList.push(command);
	}
	else if (commandToDo->getType() == Commands::CommandType::Move)
	{
		UndoMove* undoMoveCommand = new UndoMove;
		if (commandToDo->getMovedObjectsIDs().empty() == false)
		{
			//Multi
			undoMoveCommand->setup(commandToDo->getMovedObjectsIDs(), commandToDo->getMovedObjectsOriginalPositions());
			undoMoveCommand->performAction(&m_displayList);
		}
		else
		{
			//Get the display object that was moved
			int IDOfMovedObject = commandToDo->getMovedObjectsID();
			DisplayObject* movedObject;
			for (unsigned int i = 0; i < m_displayList.size(); i++)
			{
				if (IDOfMovedObject == m_displayList[i].m_ID)
				{
					movedObject = &m_displayList.at(i);
				}
			}
			undoMoveCommand->setup(IDOfMovedObject, commandToDo->getMovedObjectsOriginalPosition());
			undoMoveCommand->performAction(&m_displayList);
		}
		//Update all drag arrows
		for (unsigned int j = 0; j < m_dragArrowList.size(); j++)
		{
			m_dragArrowList[j].updateDragArrow();
		}
		Commands* command = undoMoveCommand;
		commandList.push(command);
	}
	else if (commandToDo->getType() == Commands::CommandType::UndoManipulation)
	{
		//Redo manipulation
		RedoManipulationCommand* redoManipulationCommand = new RedoManipulationCommand;
		if (commandToDo->getStoredObjects().empty() == false)
		{
			//Multi
			redoManipulationCommand->setup(commandToDo->getStoredObjects());
			redoManipulationCommand->performAction(&m_displayList);
		}
		else
		{
			redoManipulationCommand->setup(commandToDo->getStoredObject());
			redoManipulationCommand->performAction(&m_displayList);
		}
		Commands* command = redoManipulationCommand;
		commandList.push(command);
		updateAllArrowpositions();
	}
	else if (commandToDo->getType() == Commands::CommandType::RedoManipulation)
	{
		//Undo manipulation
		UndoManipulationCommand* undoManipulationCommand = new UndoManipulationCommand;
		if (commandToDo->getStoredObjects().empty() == false)
		{
			//Multi
			undoManipulationCommand->setup(commandToDo->getStoredObjects());
			undoManipulationCommand->performAction(&m_displayList);
		}
		else
		{
			//Single
			undoManipulationCommand->setup(commandToDo->getStoredObject());
			undoManipulationCommand->performAction(&m_displayList);
		}
		Commands* command = undoManipulationCommand;
		commandList.push(command);
		updateAllArrowpositions();
	}

}

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
