#include "stdafx.h"
#include "ObjectInspectorDialogue.h"

IMPLEMENT_DYNAMIC(ObjectInspectorDialogue, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(ObjectInspectorDialogue, CDialogEx)
	ON_COMMAND(IDOK, &ObjectInspectorDialogue::End)					//ok button
	ON_BN_CLICKED(IDC_BUTTON1, &ObjectInspectorDialogue::OnBnClickedLoadTexture)
	ON_BN_CLICKED(IDC_BUTTON2, &ObjectInspectorDialogue::OnBnClickedLoadModel)
END_MESSAGE_MAP()


ObjectInspectorDialogue::ObjectInspectorDialogue(CWnd* pParent)
{
}

ObjectInspectorDialogue::~ObjectInspectorDialogue()
{
}

void ObjectInspectorDialogue::DoDataExchange(CDataExchange* pDX)
{
	//Credit: https://stackoverflow.com/questions/10477786/how-to-get-numeric-value-from-edit-control
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1_XPos, XPos);
	DDX_Text(pDX, IDC_EDIT1_YPos, YPos);
	DDX_Text(pDX, IDC_EDIT1_ZPos, ZPos);
	DDX_Text(pDX, IDC_EDIT1_XScale, XScale);
	DDX_Text(pDX, IDC_EDIT1_YScale, YScale);
	DDX_Text(pDX, IDC_EDIT1_ZScale, ZScale);
	DDX_Text(pDX, IDC_EDIT1_XRotation, XRot);
	DDX_Text(pDX, IDC_EDIT1_YRotation, YRot);
	DDX_Text(pDX, IDC_EDIT1_ZRotation, ZRot);
}

void ObjectInspectorDialogue::End()
{
	//NOTE: This only works for a single object.
	//To fix this basically just loop all of this for all selected objects
	gameSystem = m_ToolSystem->getGameSystem();

	std::vector<DisplayObject*> selectedObjects = m_ToolSystem->getSelectedObjects();

	//Handle multiple objects
	if (selectedObjects.empty() == false)
	{
		UndoManipulationCommand* undoManipulationCommand = new UndoManipulationCommand;
		undoManipulationCommand->setup(selectedObjects);

		for (unsigned int i = 0; i < selectedObjects.size(); i++)
		{
			DisplayObject* selectedObject = selectedObjects.at(i);
			if (selectedObject == NULL)
			{
				DestroyWindow();
				return;
			}
			UpdateData(true);
			DirectX::XMVECTOR position = DirectX::XMVectorSet(XPos, YPos, ZPos, 0);
			//Scale can't be 0 so if it is default to what the obj has at the moment
			if (XScale <= 0)
			{
				XScale = selectedObject->m_scale.x;
			}

			if (YScale <= 0)
			{
				YScale = selectedObject->m_scale.y;
			}

			if (ZScale <= 0)
			{
				ZScale = selectedObject->m_scale.z;
			}
			DirectX::XMVECTOR scale = DirectX::XMVectorSet(XScale, YScale, ZScale, 0);
			DirectX::XMVECTOR rotation = DirectX::XMVectorSet(XRot, YRot, ZRot, 0);

			//set position
			selectedObject->m_position.x = position.m128_f32[0];
			selectedObject->m_position.y = position.m128_f32[1];
			selectedObject->m_position.z = position.m128_f32[2];

			//setorientation
			selectedObject->m_orientation.x = rotation.m128_f32[0];
			selectedObject->m_orientation.y = rotation.m128_f32[1];
			selectedObject->m_orientation.z = rotation.m128_f32[2];

			//set scale
			selectedObject->m_scale.x = scale.m128_f32[0];
			selectedObject->m_scale.y = scale.m128_f32[1];
			selectedObject->m_scale.z = scale.m128_f32[2];

			//Check model and texture now
			if (textureFile != "")
			{
				//There is a texture update
				//Load Texture
				HRESULT rs;
				CT2A convertedCString(textureFile);
				std::string convertedString(convertedCString);
				std::wstring texturewstr = StringToWCHART(convertedString.c_str());								//convect string to Wchar
				//This doesn't seem to work, I'm guessing pointer issue
				rs = DirectX::CreateDDSTextureFromFile(gameSystem->getD3DDevices().get()->GetD3DDevice(),texturewstr.c_str(), nullptr, &selectedObject->m_texture_diffuse);	//load tex into Shader resource

				//if texture fails.  load error default
				if (rs)
				{
					DirectX::CreateDDSTextureFromFile(gameSystem->getD3DDevices().get()->GetD3DDevice(), L"database/data/Error.dds", nullptr, &selectedObject->m_texture_diffuse);	//load tex into Shader resource
				}


				//apply new texture to models effect
				selectedObject->m_model->UpdateEffects([&](DirectX::IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
					{
						auto lights = dynamic_cast<DirectX::BasicEffect*>(effect);
						if (lights)
						{
							lights->SetTexture(selectedObject->m_texture_diffuse);
						}
					});

				//Set texture and model path
				std::string filename = convertedString.substr(convertedString.rfind("\\"), convertedString.length());
				std::filesystem::path relativePath = std::filesystem::relative(convertedString, "/database/data");
				selectedObject->m_texturePath = "database/data" + filename;

			}

			if (modelFile != "")
			{
				//load model
				CT2A convertedCString(modelFile);
				std::string convertedString(convertedCString);
				std::wstring modelwstr = StringToWCHART(convertedString);					//convect string to Wchar

				try
				{
					//This is very strange as it expects a dds with the same name as the model even if the dds isn't used
					selectedObject->m_model = DirectX::Model::CreateFromCMO(gameSystem->getD3DDevices().get()->GetD3DDevice(), modelwstr.c_str(),gameSystem->getfxFactory(), true);	//get DXSDK to load model "False" for LH coordinate system (maya)
					std::string filename = convertedString.substr(convertedString.rfind("\\"), convertedString.length());
					std::filesystem::path relativePath = std::filesystem::relative(convertedString, "/database/data");
					filename = convertedString.substr(convertedString.rfind("\\"), convertedString.length());
					relativePath = std::filesystem::relative(convertedString, "/database/data");
					selectedObject->m_modelPath = "database/data" + filename;
				}
				catch (const std::exception& ex)
				{
					//SOMETHING WENT WRONG
					std::printf(ex.what());
					std::cout << ex.what() << std::endl;
					return;
				}
			}
		}
		DestroyWindow();
		gameSystem->updateAllArrowpositions();
		return;
	}

	//Here is where we get the selected object and work on it
	DisplayObject* selectedObject = m_ToolSystem->getSelectedGameObject();
	if (selectedObject == NULL)
	{
		DestroyWindow();
		return;
	}
	//Command stuff
	UndoManipulationCommand* undoManipulationComand = new UndoManipulationCommand;
	undoManipulationComand->setup(selectedObject);
	Commands* command = undoManipulationComand;
	gameSystem->getCommandList().push(command);

	UpdateData(true);
	DirectX::XMVECTOR position = DirectX::XMVectorSet(XPos, YPos, ZPos, 0);
	//Scale can't be 0 so if it is default to what the obj has at the moment
	if (XScale <= 0)
	{
		XScale = selectedObject->m_scale.x;
	}
	
	if (YScale <= 0)
	{
		YScale = selectedObject->m_scale.y;
	}

	if (ZScale <= 0)
	{
		ZScale = selectedObject->m_scale.z;
	}
	DirectX::XMVECTOR scale = DirectX::XMVectorSet(XScale, YScale, ZScale, 0);
	DirectX::XMVECTOR rotation = DirectX::XMVectorSet(XRot, YRot, ZRot, 0);

	//set position
	selectedObject->m_position.x = position.m128_f32[0];
	selectedObject->m_position.y = position.m128_f32[1];
	selectedObject->m_position.z = position.m128_f32[2];

	//setorientation
	selectedObject->m_orientation.x = rotation.m128_f32[0];
	selectedObject->m_orientation.y = rotation.m128_f32[1];
	selectedObject->m_orientation.z = rotation.m128_f32[2];

	//set scale
	selectedObject->m_scale.x = scale.m128_f32[0];
	selectedObject->m_scale.y = scale.m128_f32[1];
	selectedObject->m_scale.z = scale.m128_f32[2];

	//Check model and texture now
	if (textureFile != "")
	{
		//There is a texture update
		//Load Texture
		HRESULT rs;
		CT2A convertedCString(textureFile);
		std::string convertedString(convertedCString);
		std::wstring texturewstr = StringToWCHART(convertedString.c_str());								//convect string to Wchar
		//This doesn't seem to work, I'm guessing pointer issue
		rs = DirectX::CreateDDSTextureFromFile(gameSystem->getD3DDevices().get()->GetD3DDevice(), texturewstr.c_str(), nullptr, &selectedObject->m_texture_diffuse);	//load tex into Shader resource

		//if texture fails.  load error default
		if (rs)
		{
			DirectX::CreateDDSTextureFromFile(gameSystem->getD3DDevices().get()->GetD3DDevice(), L"database/data/Error.dds", nullptr, &selectedObject->m_texture_diffuse);	//load tex into Shader resource
		}


		//apply new texture to models effect
		selectedObject->m_model->UpdateEffects([&](DirectX::IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
			{
				auto lights = dynamic_cast<DirectX::BasicEffect*>(effect);
				if (lights)
				{
					lights->SetTexture(selectedObject->m_texture_diffuse);
				}
			});

		//Set texture and model path
		std::string filename = convertedString.substr(convertedString.rfind("\\"), convertedString.length());
		std::filesystem::path relativePath = std::filesystem::relative(convertedString, "/database/data");
		selectedObject->m_texturePath = "database/data" + filename;
	}

	if (modelFile != "")
	{
		//load model
		CT2A convertedCString(modelFile);
		std::string convertedString(convertedCString);
		std::wstring modelwstr = StringToWCHART(convertedString);					//convect string to Wchar

		try
		{
			//This is very strange as it expects a dds with the same name as the model even if the dds isn't used
			selectedObject->m_model = DirectX::Model::CreateFromCMO(gameSystem->getD3DDevices().get()->GetD3DDevice(), modelwstr.c_str(), gameSystem->getfxFactory(), true);	//get DXSDK to load model "False" for LH coordinate system (maya)
			std::string filename = convertedString.substr(convertedString.rfind("\\"), convertedString.length());
			std::filesystem::path relativePath = std::filesystem::relative(convertedString, "/database/data");
			filename = convertedString.substr(convertedString.rfind("\\"), convertedString.length());
			relativePath = std::filesystem::relative(convertedString, "/database/data");
			selectedObject->m_modelPath = "database/data" + filename;
		}
		catch (const std::exception& ex)
		{
			//SOMETHING WENT WRONG
			std::printf(ex.what());
			std::cout << ex.what() << std::endl;
			return;
		}
	}

	gameSystem->updateAllArrowpositions();

	DestroyWindow();
}

BOOL ObjectInspectorDialogue::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	textureFile = "";
	modelFile = "";
	gameSystem = m_ToolSystem->getGameSystem();
	return TRUE;
}

void ObjectInspectorDialogue::PostNcDestroy()
{
}

void ObjectInspectorDialogue::setToolSystem(ToolMain* toolsystem)
{
	m_ToolSystem = toolsystem;
}

void ObjectInspectorDialogue::OnBnClickedLoadTexture()
{
	//Credit: https://www.programmerall.com/article/50391274688/ 
	CFileDialog dlg(TRUE, _T("*.dds"), NULL, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("Image Files(*.dds)|*.dds|"), NULL);
	dlg.m_ofn.lpstrTitle = _T("Select Image");
	CString filename;
	if (dlg.DoModal() == IDOK)
	{
		POSITION fileNamesPosition = dlg.GetStartPosition();
		filename = dlg.GetNextPathName(fileNamesPosition);
		textureFile = filename;
	}

}

void ObjectInspectorDialogue::OnBnClickedLoadModel()
{
	//Credit: https://www.programmerall.com/article/50391274688/ 
	CFileDialog dlg(TRUE, _T("*.cmo"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("Model(*.cmo)|*.cmo|"), NULL);
	dlg.m_ofn.lpstrTitle = _T("Select Model");
	CString filename;
	if (dlg.DoModal() == IDOK)
	{
		POSITION fileNamesPosition = dlg.GetStartPosition();
		filename = dlg.GetNextPathName(fileNamesPosition);
		modelFile = filename;
	}
}
