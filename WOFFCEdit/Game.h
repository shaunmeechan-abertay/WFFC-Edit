//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "SceneObject.h"
#include "DisplayObject.h"
#include "DisplayChunk.h"
#include "ChunkObject.h"
#include "InputCommands.h"
#include "DeleteCommand.h"
#include "CreateCommand.h"
#include "CopyCommand.h"
#include "UndoMove.h"
#include "MoveCommand.h"
#include "UndoManipulationCommand.h"
#include "RedoManipulationCommand.h"
#include "DragArrow.h"
#include "Camera.h"
#include <vector>
#include <list>
#include <limits>
#include <memory>
#include <string>
#include <stack>

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify
{
public:

	Game();
	~Game();

	// Initialization and management
	void Initialize(HWND window, int width, int height);
	void SetGridState(bool state);

	// Basic game loop
	void Tick(InputCommands * Input);
	void Render();

	// Rendering helpers
	void Clear();

	// IDeviceNotify
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored() override;

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowSizeChanged(int width, int height);

	//tool specific
	void BuildDisplayList(std::vector<SceneObject> * SceneGraph); //note vector passed by reference 
	void BuildDisplayChunk(ChunkObject *SceneChunk);
	void SaveDisplayChunk(ChunkObject *SceneChunk);	//saves geometry et al
	void ClearDisplayList();

	int MousePicking();

	//Click and drag
	DragArrow* selectedArrow;
	void clickAndDrag();
	void checkForDragArrow();
	void dragFinished();
	float mouseYOnClick;
	float mouseXOnClick;
	//DO NOT USE XMVECTOR HERE! YES IT MAKES SENSE BUT CAUSES A COMPILER ERROR THAT BREAKS THE COMPALATION OF COMMANDS.H
	float objectsOriginalPositionX;
	float objectsOriginalPositionY;
	float objectsOriginalPositionZ;
	//multi object support
	std::vector<float> objectsOriginalPositionsX;
	std::vector<float> objectsOriginalPositionsY;
	std::vector<float> objectsOriginalPositionsZ;
	std::vector<int> objectIDs;

	void setID(int newID);

	void copyObject();
	//We only want one of these as you can only "store" one object/objects at a time
	CopyCommand copyCommand;
	void pasteObject();
	//Focus function
	void focusOnItem();
	void archballL();
	void archballR();
	bool shouldResetOrientation = false;
	
	void CreateNewObject(std::string texturespath, std::string modelspath, DirectX::XMVECTOR position, DirectX::XMVECTOR scale, DirectX::XMVECTOR rotation);
	std::vector<SceneObject> getDisplayList();

	//Functions for selected objects vector
	void pushBackNewSelectedObject(DisplayObject* newObject);
	void eraseSelectedObject(DisplayObject* newObject);

	//Function for dealing with drag arrows
	void cleanupAllArrows();

	//Function that updates arrow positions
	void updateAllArrowpositions();

	//Function for handling camera movement with a mouse
	void mouseCameraMovement();

	//Selected object/objects getters
	DisplayObject* getSelectedObject();
	std::vector<DisplayObject*> getSelectedObjects();

	std::shared_ptr<DX::DeviceResources> getD3DDevices();
	DirectX::IEffectFactory& getfxFactory();

	std::stack<Commands*>* getCommandList();

	//Terrain functions (called from MFC)
	void flattenTerrain();
	void saveTerrain();
	void generateNewTerrain();

#ifdef DXTK_AUDIO
	void NewAudioDevice();
#endif

private:

	void Update(DX::StepTimer const& timer);

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	//Command functions
	void undoAction();
	void RedoAction();

	void XM_CALLCONV DrawGrid(DirectX::FXMVECTOR xAxis, DirectX::FXMVECTOR yAxis, DirectX::FXMVECTOR origin, size_t xdivs, size_t ydivs, DirectX::GXMVECTOR color);

	//tool specific
	std::vector<DisplayObject>			m_displayList;
	std::vector<DragArrow>			m_dragArrowList;
	DisplayChunk						m_displayChunk;
	InputCommands						m_InputCommands;
	RECT								m_ScreenDimensions;
	//functionality
	float								m_movespeed;
	bool								m_DragStarted;

	//camera
	Camera								m_camera;

	//control variables
	bool m_grid;							//grid rendering on / off

	//Command variables
	std::stack<Commands*> commandList;
	std::stack<Commands*> UndonecommandList;
	//This should emulate press
	bool inputDown = false;
	//ID from MFC
	int ID = 0;
	DisplayObject* selectedObject;
	//Vector of selected objects
	std::vector<DisplayObject*> selectedObjects;
	// Device resources.
    std::shared_ptr<DX::DeviceResources>    m_deviceResources;

	//Setters for our selected object/objects
	void setSelectedObject(DisplayObject* newObject);
	void setSelectedObjects(std::vector<DisplayObject*> newObjects);

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

    // Input devices.
    std::unique_ptr<DirectX::GamePad>       m_gamePad;
    std::unique_ptr<DirectX::Keyboard>      m_keyboard;
    std::unique_ptr<DirectX::Mouse>         m_mouse;

    // DirectXTK objects.
    std::unique_ptr<DirectX::CommonStates>                                  m_states;
    std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;
    std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
    std::unique_ptr<DirectX::GeometricPrimitive>                            m_shape;
    std::unique_ptr<DirectX::Model>                                         m_model;
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
    std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
    std::unique_ptr<DirectX::SpriteFont>                                    m_font;

#ifdef DXTK_AUDIO
    std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
    std::unique_ptr<DirectX::WaveBank>                                      m_waveBank;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect1;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect2;
#endif

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture2;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;

#ifdef DXTK_AUDIO
    uint32_t                                                                m_audioEvent;
    float                                                                   m_audioTimerAcc;

    bool                                                                    m_retryDefault;
#endif

    DirectX::SimpleMath::Matrix                                             m_world;
    DirectX::SimpleMath::Matrix                                             m_view;
    DirectX::SimpleMath::Matrix                                             m_projection;


};

std::wstring StringToWCHART(std::string s);