#include "stdafx.h"
#include "GUI.h"
#include "CgalBooleanOperations.h"

#include "DrawMesh.h"
#include "font_awesome.hpp"
#include "lato.hpp"
#include "IconsFontAwesome5.h"
#include "GuiUtilities.h"
#include "ImGuiFileDialog.h"
#include "InputManager.h"
#include "Renderer.h"
#include <chrono>

AlgGeom::GUI::GUI()
{
	_appState = InputManager::getApplicationState();
	_cameraGuiAdapter = new CameraGuiAdapter;
	_currentGizmoOperation = ImGuizmo::TRANSLATE;
	_currentGizmoMode = ImGuizmo::WORLD;
	_fileDialog = NONE;
	_lastDirectory = DEFAULT_DIRECTORY;
	_modelCompSelected = nullptr;

	_showMenuButtons = new bool[NUM_GUI_MENU_BUTTONS];
	for (int idx = 0; idx < NUM_GUI_MENU_BUTTONS; ++idx) _showMenuButtons[idx] = false;
	_showMenuButtons[MenuButtons::MODELS] = true;
	_showMenuButtons[MenuButtons::RENDERING] = true;
}

AlgGeom::GUI::~GUI()
{
	delete[] _showMenuButtons;

	ImGui::DestroyContext();
}

void AlgGeom::GUI::editTransform(ImGuizmo::OPERATION& operation, ImGuizmo::MODE& mode)
{
	if (ImGui::RadioButton("Translate", operation == ImGuizmo::TRANSLATE))
	{
		operation = ImGuizmo::TRANSLATE;
	}

	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", operation == ImGuizmo::ROTATE))
	{
		operation = ImGuizmo::ROTATE;
	}

	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", operation == ImGuizmo::SCALE))
	{
		operation = ImGuizmo::SCALE;
	}

	if (operation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mode == ImGuizmo::LOCAL))
		{
			mode = ImGuizmo::LOCAL;
		}

		ImGui::SameLine();
		if (ImGui::RadioButton("World", mode == ImGuizmo::WORLD))
		{
			mode = ImGuizmo::WORLD;
		}
	}
}

void AlgGeom::GUI::loadFonts()
{
	ImFontConfig cfg;
	ImGuiIO& io = ImGui::GetIO();

	std::copy_n("Lato", 5, cfg.Name);
	io.Fonts->AddFontFromMemoryCompressedBase85TTF(LatoFont::lato_compressed_data_base85, 13.0f, &cfg);

	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	cfg.MergeMode = true;
	cfg.PixelSnapH = true;
	cfg.GlyphMinAdvanceX = 20.0f;
	cfg.GlyphMaxAdvanceX = 20.0f;
	std::copy_n("FontAwesome", 12, cfg.Name);

	io.Fonts->AddFontFromFileTTF("Assets/Fonts/fa-regular-400.ttf", 12.0f, &cfg, icons_ranges);
	io.Fonts->AddFontFromFileTTF("Assets/Fonts/fa-solid-900.ttf", 12.0f, &cfg, icons_ranges);
}

void AlgGeom::GUI::loadImGUIStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	// Fondo general muy oscuro (estilo Blender)
	colors[ImGuiCol_WindowBg]             = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_ChildBg]              = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	colors[ImGuiCol_PopupBg]              = ImVec4(0.18f, 0.18f, 0.18f, 0.94f);
	colors[ImGuiCol_Border]               = ImVec4(0.25f, 0.25f, 0.25f, 0.50f);
	colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	// Headers naranja
	colors[ImGuiCol_Header]               = ImVec4(0.90f, 0.55f, 0.15f, 0.55f);
	colors[ImGuiCol_HeaderHovered]        = ImVec4(1.00f, 0.65f, 0.20f, 0.80f);
	colors[ImGuiCol_HeaderActive]         = ImVec4(0.95f, 0.60f, 0.15f, 1.00f);

	// Botones naranja oscuro
	colors[ImGuiCol_Button]               = ImVec4(0.80f, 0.45f, 0.10f, 0.80f);
	colors[ImGuiCol_ButtonHovered]        = ImVec4(0.95f, 0.55f, 0.15f, 1.00f);
	colors[ImGuiCol_ButtonActive]         = ImVec4(1.00f, 0.65f, 0.20f, 1.00f);

	// Frames
	colors[ImGuiCol_FrameBg]              = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_FrameBgActive]        = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

	// Tabs
	colors[ImGuiCol_Tab]                  = ImVec4(0.20f, 0.20f, 0.20f, 0.86f);
	colors[ImGuiCol_TabHovered]           = ImVec4(0.90f, 0.55f, 0.15f, 0.80f);
	colors[ImGuiCol_TabActive]            = ImVec4(0.85f, 0.50f, 0.12f, 1.00f);
	colors[ImGuiCol_TabUnfocused]         = ImVec4(0.15f, 0.15f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive]   = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

	// Texto
	colors[ImGuiCol_Text]                 = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	colors[ImGuiCol_TextDisabled]         = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.90f, 0.55f, 0.15f, 0.35f);

	// Title
	colors[ImGuiCol_TitleBg]              = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgActive]        = ImVec4(0.90f, 0.55f, 0.15f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.12f, 0.12f, 0.12f, 0.51f);

	// Misc
	colors[ImGuiCol_CheckMark]            = ImVec4(0.95f, 0.60f, 0.15f, 1.00f);
	colors[ImGuiCol_SliderGrab]           = ImVec4(0.90f, 0.55f, 0.15f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]     = ImVec4(1.00f, 0.65f, 0.20f, 1.00f);
	colors[ImGuiCol_Separator]            = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
	colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.90f, 0.55f, 0.15f, 0.78f);
	colors[ImGuiCol_SeparatorActive]      = ImVec4(0.95f, 0.60f, 0.15f, 1.00f);
	colors[ImGuiCol_ResizeGrip]           = ImVec4(0.90f, 0.55f, 0.15f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.95f, 0.60f, 0.15f, 0.67f);
	colors[ImGuiCol_ResizeGripActive]     = ImVec4(1.00f, 0.65f, 0.20f, 0.95f);
	colors[ImGuiCol_DragDropTarget]       = ImVec4(0.95f, 0.60f, 0.15f, 0.90f);
	colors[ImGuiCol_NavHighlight]         = ImVec4(0.90f, 0.55f, 0.15f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]= ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]    = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.10f, 0.10f, 0.10f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_PlotLines]            = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]     = ImVec4(1.00f, 0.80f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotHistogram]        = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.80f, 0.20f, 1.00f);

	// Redondeo y padding
	style.WindowRounding    = 4.0f;
	style.ChildRounding     = 4.0f;
	style.FrameRounding     = 4.0f;
	style.GrabRounding      = 3.0f;
	style.TabRounding       = 4.0f;
	style.PopupRounding     = 4.0f;
	style.ScrollbarRounding = 4.0f;
	style.WindowPadding     = ImVec2(10, 10);
	style.FramePadding      = ImVec2(6, 4);
	style.ItemSpacing       = ImVec2(8, 6);
	style.ItemInnerSpacing  = ImVec2(6, 4);

	this->loadFonts();
}

void AlgGeom::GUI::processSelectedFile(FileDialog fileDialog, const std::string& filename, SceneContent* sceneContent)
{
	if (fileDialog == FileDialog::OPEN_MESH)
	{
		Model3D* model = (new DrawMesh())->loadModelOBJ(filename);
		model->moveGeometryToOrigin();
		sceneContent->addNewModel(model);
		sceneContent->_currentModelPath = filename;
		sceneContent->_drawA_ref = model;
	}
	else if (fileDialog == FileDialog::OPEN_MESH_B)
	{
		Model3D* model = (new DrawMesh())->loadModelOBJ(filename);
		model->moveGeometryToOrigin();
		sceneContent->addNewModel(model);
		sceneContent->_currentModelPathB = filename;
		sceneContent->_drawB_ref = model;
	}
	else if (fileDialog == FileDialog::OPEN_BOOL_A)
	{
		sceneContent->loadBooleanModelA(filename);
	}
	else if (fileDialog == FileDialog::OPEN_BOOL_B)
	{
		sceneContent->loadBooleanModelB(filename);
	}
	else if (fileDialog == FileDialog::SAVE_BOOL_RESULT)
	{
		sceneContent->saveBooleanResult(filename);
	}
}

void AlgGeom::GUI::renderGuizmo(Model3D::Component* component, SceneContent* sceneContent)
{
	if (component && _showMenuButtons[MenuButtons::MODELS])
	{
		if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_T))
			_currentGizmoOperation = ImGuizmo::TRANSLATE;

		if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R))
			_currentGizmoOperation = ImGuizmo::ROTATE;

		if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S))
			_currentGizmoOperation = ImGuizmo::SCALE;

		const mat4 viewMatrix = mat4(sceneContent->_camera[_appState->_selectedCamera]->getViewMatrix());
		const mat4 projectionMatrix = mat4(sceneContent->_camera[_appState->_selectedCamera]->getProjectionMatrix());
		Model3D* model = sceneContent->getModel(component);

		if (model)
		{
			mat4 modelMatrix = model->getModelMatrix();

			ImGuiIO& io = ImGui::GetIO();
			ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
			ImGuizmo::Manipulate(&(viewMatrix[0][0]), &(projectionMatrix[0][0]), _currentGizmoOperation, _currentGizmoMode, &(modelMatrix[0][0]), nullptr, nullptr);

			model->setModelMatrix(modelMatrix);

			static bool wasUsingGizmo = false;
			bool isUsing = ImGuizmo::IsUsing();

			if (isUsing && sceneContent->_isPr4Active) {
				sceneContent->update_pr4(true);
			}
			if (wasUsingGizmo && !isUsing && sceneContent->_isPr4Active) {
				sceneContent->update_pr4(false);
				
				if (sceneContent->_isBruteForceActive) {
					std::cout << "\n--- Ejecutando Comparativa (Paso 7) ---" << std::endl;
					sceneContent->runPr4BruteForce();
				}
			}
			wasUsingGizmo = isUsing;
		}
	}
}

void AlgGeom::GUI::initialize(GLFWwindow* window, const int openGLMinorVersion)
{
	const std::string openGLVersion = "#version 4" + std::to_string(openGLMinorVersion) + "0 core";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	this->loadImGUIStyle();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(openGLVersion.c_str());
}

void AlgGeom::GUI::render(SceneContent* sceneContent)
{
	// Actualizar animación de RP5 (Gift Wrapping)
	if (sceneContent) {
		sceneContent->update_pr5();
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	for (int menuButtonIdx = 0; menuButtonIdx < NUM_GUI_MENU_BUTTONS; ++menuButtonIdx)
	{
		MenuButtons button = static_cast<MenuButtons>(menuButtonIdx);

		if (_showMenuButtons[button])
		{
			switch (button)
			{
			case MenuButtons::RENDERING:
				this->showRenderingMenu(sceneContent);
				break;
			case MenuButtons::MODELS:
				this->showModelMenu(sceneContent);
				break;
			case MenuButtons::CAMERA:
				this->showCameraMenu(sceneContent);
				break;
			case MenuButtons::LIGHT:
				this->showLightMenu(sceneContent);
				break;
			case MenuButtons::SCREENSHOT:
				this->showScreenshotMenu(sceneContent);
				break;
			}
		}
	}

	this->showFileDialog(sceneContent);

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(ICON_FA_COG " Ventanas"))
		{
			ImGui::MenuItem(ICON_FA_CUBE " Proyecto Final", NULL, &_showMenuButtons[MenuButtons::MODELS]);
			ImGui::MenuItem(ICON_FA_DRAW_POLYGON " Rendering", NULL, &_showMenuButtons[MenuButtons::RENDERING]);
			ImGui::EndMenu();
		}

		ImGui::SameLine(0, 20);
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::EndMainMenuBar();
	}

	this->renderGuizmo(_modelCompSelected, sceneContent);

	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void AlgGeom::GUI::showCameraMenu(SceneContent* sceneContent)
{
	static Camera* cameraSelected = nullptr;

	if (ImGui::Begin("Lights", &this->_showMenuButtons[MenuButtons::CAMERA], ImGuiWindowFlags_None))
	{
		GuiUtilities::leaveSpace(2);
		ImGui::BeginChild("Camera List", ImVec2(200, 0), true);

		for (int cameraIdx = 0; cameraIdx < sceneContent->_camera.size(); ++cameraIdx)
		{
			const std::string cameraName = "Camera " + std::to_string(cameraIdx);
			if (ImGui::Selectable(cameraName.c_str(), cameraSelected == sceneContent->_camera[cameraIdx].get()))
			{
				cameraSelected = sceneContent->_camera[cameraIdx].get();
				_appState->_selectedCamera = cameraIdx;
			}
		}

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGui::BeginChild("Model Component View", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));		// Leave room for 1 line below us

		if (cameraSelected)
		{
			_cameraGuiAdapter->setCamera(cameraSelected);
			_cameraGuiAdapter->renderGuiObject();
		}

		ImGui::EndChild();
		ImGui::EndGroup();

		ImGui::End();
	}
}

void AlgGeom::GUI::showFileDialog(SceneContent* sceneContent)
{
	if (_fileDialog != NONE)
	{
		if (std::filesystem::exists(_lastDirectory))
			_lastDirectory = DEFAULT_DIRECTORY;

		uint16_t iFileDialog = static_cast<uint16_t>(_fileDialog);
		ImGuiFileDialog::Instance()->OpenDialog(FILE_DIALOG_TEXT[iFileDialog], "Select a file", FILE_DIALOG_EXTENSION[iFileDialog].c_str());

		if (ImGuiFileDialog::Instance()->Display(FILE_DIALOG_TEXT[iFileDialog]))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				_path = ImGuiFileDialog::Instance()->GetFilePathName();
				_lastDirectory = _path.substr(0, _path.find_last_of('\\'));

				this->processSelectedFile(_fileDialog, _path, sceneContent);
				_fileDialog = NONE;
			}

			ImGuiFileDialog::Instance()->Close();
			_fileDialog = NONE;
		}
	}
}

void AlgGeom::GUI::showLightMenu(SceneContent* sceneContent)
{
	ImGui::SetNextWindowSize(ImVec2(800, 440), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Lights", &this->_showMenuButtons[MenuButtons::LIGHT], ImGuiWindowFlags_None))
	{
		GuiUtilities::leaveSpace(1);
		ImGui::Text("Light Properties");
		ImGui::Separator();
		GuiUtilities::leaveSpace(2);

		AABB aabb = sceneContent->_sceneAABB;
		float maxCoordinate = std::max(std::max(aabb.max().x, aabb.max().y), aabb.max().z);
		float minCoordinate = std::min(std::min(aabb.min().x, aabb.min().y), aabb.min().z);

		ImGui::SliderFloat3("Light Position", &_appState->_lightPosition[0], minCoordinate, maxCoordinate);
		ImGui::ColorEdit3("Ia", &_appState->_Ia[0]);
		ImGui::ColorEdit3("Id", &_appState->_Id[0]);
		ImGui::ColorEdit3("Is", &_appState->_Is[0]);

		ImGui::End();
	}
}

void AlgGeom::GUI::showModelMenu(SceneContent* sceneContent)
{
	ImGui::SetNextWindowSize(ImVec2(800, 440), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Proyecto Final - Booleanas CGAL", &this->_showMenuButtons[MenuButtons::MODELS], ImGuiWindowFlags_None))
	{
		// Botones legacy PR1-PR5 ocultos (codigo preservado)
#if 0
		ImGui::SameLine();
		if (ImGui::Button("PR1 A")) { sceneContent->clearScene(); sceneContent->buildPr1a(); _modelCompSelected = nullptr; }
		ImGui::SameLine();
		if (ImGui::Button("PR1 B")) { sceneContent->clearScene(); sceneContent->buildPr1b(); _modelCompSelected = nullptr; }
		ImGui::SameLine();
		if (ImGui::Button("PR2 A")) { sceneContent->clearScene(); sceneContent->buildPr2a(); _modelCompSelected = nullptr; }
		ImGui::SameLine();
		if (ImGui::Button("PR2 B")) { sceneContent->clearScene(); sceneContent->buildPr2b(); _modelCompSelected = nullptr; }
		ImGui::SameLine();
		if (ImGui::Button("PR2 C")) { sceneContent->clearScene(); sceneContent->buildPr2c(); _modelCompSelected = nullptr; }
		ImGui::SameLine();
		if (ImGui::Button("PR2 D")) { sceneContent->clearScene(); sceneContent->buildPr2d(); _modelCompSelected = nullptr; }
		ImGui::SameLine();
		if (ImGui::Button("Delaunay")) { sceneContent->clearScene(); sceneContent->buildDelaunay(); _modelCompSelected = nullptr; }
		ImGui::SameLine();
		if (ImGui::Button("PR3 A")) { /* ... */ }
		ImGui::SameLine();
		if (ImGui::Button("PR4B")) { /* ... */ }
		ImGui::SameLine();
		if (ImGui::Button("Nube PR5")) { sceneContent->generateCloudPr5(); _modelCompSelected = nullptr; }
		ImGui::SameLine();
		if (ImGui::Button("Modelo PR5")) { /* ... */ }
#endif

		ImGui::SameLine();
		if (ImGui::Button("PROYECTO FINAL"))
		{
			sceneContent->clearScene();
			sceneContent->buildPr6();
			_modelCompSelected = nullptr;
		}

		ImGui::SameLine();
		if (ImGui::Button("Limpiar Escena"))
		{
			sceneContent->clearScene();
			_modelCompSelected = nullptr;
		}

        // Panel de ejecucion del Proyecto Final (PR6)
        if (sceneContent->_isPr6Active) {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1.0f, 0.65f, 0.20f, 1.0f), "PROYECTO FINAL - Operaciones Booleanas CGAL");

            if (ImGui::Button("Cargar Modelo A")) {
                _fileDialog = FileDialog::OPEN_BOOL_A;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cargar Modelo B")) {
                _fileDialog = FileDialog::OPEN_BOOL_B;
            }
            ImGui::SameLine();
            if (ImGui::Button("Limpiar Proyecto")) {
                sceneContent->clearBooleanScene();
                _modelCompSelected = nullptr;
            }

            // Info de diagnosticos de modelos cargados
            if (sceneContent->_cgalBool) {
                ImGui::Spacing();
                bool validA = sceneContent->_cgalBool->hasA() && sceneContent->_cgalBool->isValidA();
                bool validB = sceneContent->_cgalBool->hasB() && sceneContent->_cgalBool->isValidB();
                ImVec4 colorOk(0.0f, 1.0f, 0.0f, 1.0f);
                ImVec4 colorBad(1.0f, 0.0f, 0.0f, 1.0f);

                if (sceneContent->_cgalBool->hasA()) {
                    ImGui::TextColored(validA ? colorOk : colorBad, "Modelo A: %zu vertices, %zu caras (%s)",
                        sceneContent->_cgalBool->getAVertices(),
                        sceneContent->_cgalBool->getAFaces(),
                        validA ? "VALIDO" : "NO VALIDO - debe ser cerrado");
                } else {
                    ImGui::TextColored(colorBad, "Modelo A: no cargado");
                }

                if (sceneContent->_cgalBool->hasB()) {
                    ImGui::TextColored(validB ? colorOk : colorBad, "Modelo B: %zu vertices, %zu caras (%s)",
                        sceneContent->_cgalBool->getBVertices(),
                        sceneContent->_cgalBool->getBFaces(),
                        validB ? "VALIDO" : "NO VALIDO - debe ser cerrado");
                } else {
                    ImGui::TextColored(colorBad, "Modelo B: no cargado");
                }

                if (!validA || !validB) {
                    ImGui::TextColored(colorBad, "IMPORTANTE: los modelos deben ser mallas cerradas (watertight).");
                    ImGui::TextColored(colorBad, "Usa cubos, esferas, o modelos sin agujeros.");
                }
            }

            ImGui::Spacing();
            if (ImGui::Button("Union")) {
                sceneContent->runBooleanUnion();
                _modelCompSelected = nullptr;
            }
            ImGui::SameLine();
            if (ImGui::Button("Interseccion")) {
                sceneContent->runBooleanIntersection();
                _modelCompSelected = nullptr;
            }
            ImGui::SameLine();
            if (ImGui::Button("Diferencia A-B")) {
                sceneContent->runBooleanDifferenceAB();
                _modelCompSelected = nullptr;
            }
            ImGui::SameLine();
            if (ImGui::Button("Diferencia B-A")) {
                sceneContent->runBooleanDifferenceBA();
                _modelCompSelected = nullptr;
            }
            ImGui::SameLine();
            if (ImGui::Button("Volver a modelos")) {
                sceneContent->showBooleanInputs();
                _modelCompSelected = nullptr;
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Oculta el resultado y muestra A y B para moverlos/rotarlos.");
            }

            ImGui::Spacing();
            ImGui::PushItemWidth(250);
            ImGui::SliderFloat("Ratio simplificacion (1.0 = sin cambios)", &sceneContent->_pr6SimplifyRatio, 0.01f, 1.0f);
            ImGui::PopItemWidth();
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Valores menores reducen mas la malla.\nSe aplica al RESULTADO de la operacion booleana.");
            }
            ImGui::SameLine();
            if (ImGui::Button("Aplicar reduccion")) {
                sceneContent->simplifyBooleanResult(sceneContent->_pr6SimplifyRatio);
                _modelCompSelected = nullptr;
            }
            ImGui::SameLine();
            static char saveName[128] = "resultado";
            ImGui::PushItemWidth(120);
            ImGui::InputText("##saveName", saveName, IM_ARRAYSIZE(saveName));
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (ImGui::Button("Guardar .obj")) {
                std::string outPath = std::string(saveName) + ".obj";
                sceneContent->saveBooleanResult(outPath);
            }

            if (sceneContent->_cgalBool && sceneContent->_cgalBool->hasResult()) {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Resultado: %zu vertices, %zu caras",
                    sceneContent->_cgalBool->getResultVertices(),
                    sceneContent->_cgalBool->getResultFaces());
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Sin resultado aun. Ejecuta una operacion booleana.");
            }
            ImGui::Separator();
        }

		GuiUtilities::leaveSpace(1);

		ImGui::BeginChild("Components", ImVec2(200, 0), true);

		bool isSelectedValid = false;

		for (int modelIdx = 0; modelIdx < sceneContent->_model.size(); ++modelIdx)
		{
			for (int compIdx = 0; compIdx < sceneContent->_model[modelIdx]->_components.size(); ++compIdx)
			{
				if (_modelCompSelected == sceneContent->_model[modelIdx]->_components[compIdx].get())
				{
					isSelectedValid = true;
				}

				const std::string compName = sceneContent->_model[modelIdx]->getName() + ", Comp. " + std::to_string(compIdx) + "##" + std::to_string(modelIdx) + "_" + std::to_string(compIdx);
				if (ImGui::Selectable(compName.c_str(), _modelCompSelected == sceneContent->_model[modelIdx]->_components[compIdx].get()))
				{
					_modelCompSelected = sceneContent->_model[modelIdx]->_components[compIdx].get();
					isSelectedValid = true;
				}
			}
		}

		if (!isSelectedValid) 
		{
			_modelCompSelected = nullptr;
		}

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGui::BeginChild("Model Component View", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));		// Leave room for 1 line below us

		if (_modelCompSelected)
		{
			ImGui::Text("Visibility");
			ImGui::Separator();

			ImGui::Checkbox("Enabled", &_modelCompSelected->_enabled);

			Model3D* parentModel = sceneContent->getModel(_modelCompSelected);
			if (parentModel)
			{
				if (ImGui::Button("Centrar camara en modelo"))
				{
					Camera* camera = Renderer::getInstance()->getCamera();
					if (camera) camera->track(parentModel);
				}
			}

			GuiUtilities::leaveSpace(4);
			ImGui::Text("Material");
			ImGui::Separator();

			ImGui::Checkbox("Use Uniform Color", &_modelCompSelected->_material._useUniformColor);
			ImGui::ColorEdit4("Kd Color", &_modelCompSelected->_material._kdColor[0]);
			ImGui::ColorEdit3("Ks Color", &_modelCompSelected->_material._ksColor[0]);
			ImGui::SliderFloat("Metallic", &_modelCompSelected->_material._metallic, .0f, 1.0f);
			ImGui::SliderFloat("Roughness exponent", &_modelCompSelected->_material._roughnessK, .0f, 1.0f);
			ImGui::ColorEdit3("Line Color", &_modelCompSelected->_material._lineColor[0]);
			ImGui::ColorEdit3("Point Color", &_modelCompSelected->_material._pointColor[0]);

			GuiUtilities::leaveSpace(2);
			ImGui::Text("Topology");
			ImGui::Separator();

			static const char* topologyTitle[] = { "Point Cloud", "Wireframe", "Triangle Mesh" };
			for (int topologyIdx = 0; topologyIdx < VAO::NUM_IBOS; ++topologyIdx)
			{
				ImGui::Checkbox(topologyTitle[topologyIdx], &_modelCompSelected->_activeRendering[topologyIdx]);
				if (topologyIdx < VAO::NUM_IBOS - 1) ImGui::SameLine(0, 10);
			}
			ImGui::SliderFloat("Line Width", &_modelCompSelected->_lineWidth, .0f, 10.0f);
			ImGui::SliderFloat("Point Size", &_modelCompSelected->_pointSize, .0f, 10.0f);

			GuiUtilities::leaveSpace(2);

			ImGui::Text("Matrices");
			ImGui::Separator();

			this->editTransform(_currentGizmoOperation, _currentGizmoMode);

			GuiUtilities::leaveSpace(2);
			ImGui::Text("Transformacion Numerica");
			ImGui::Separator();

			Model3D* model = sceneContent->getModel(_modelCompSelected);
			if (model)
			{
				mat4 m = model->getModelMatrix();
				vec3 translation = vec3(m[3]);
				vec3 scale = vec3(glm::length(vec3(m[0])), glm::length(vec3(m[1])), glm::length(vec3(m[2])));

				glm::quat rotation;
				vec3 skew;
				vec4 perspective;
				glm::decompose(m, scale, rotation, translation, skew, perspective);
				vec3 euler = glm::eulerAngles(rotation);

				bool changed = false;
				vec3 eulerDeg = glm::degrees(euler);
				changed |= ImGui::InputFloat3("Traslacion", &translation[0]);
				changed |= ImGui::InputFloat3("Rotacion (grados)", &eulerDeg[0]);
				changed |= ImGui::InputFloat3("Escala", &scale[0]);

				if (changed)
				{
					mat4 newMatrix = glm::translate(mat4(1.0f), translation)
					               * glm::mat4_cast(glm::quat(glm::radians(eulerDeg)))
					               * glm::scale(mat4(1.0f), scale);
					model->setModelMatrix(newMatrix);
				}
			}
		}

		ImGui::EndChild();
		ImGui::EndGroup();
	}

	ImGui::End();
}

void AlgGeom::GUI::showRenderingMenu(SceneContent* sceneContent)
{
	if (ImGui::Begin("Rendering", &_showMenuButtons[RENDERING]))
	{
		ImGui::ColorEdit3("Background color", &_appState->_backgroundColor[0]);
		ImGui::SliderFloat("Gamma", &_appState->_gamma, 1.0f, 5.0f);

		GuiUtilities::leaveSpace(2);
		ImGui::Text("Colores globales");
		ImGui::Separator();

		bool globalColorChanged = false;
		globalColorChanged |= ImGui::ColorEdit3("Wireframe", &_appState->_globalWireframeColor[0]);
		globalColorChanged |= ImGui::ColorEdit4("Triangulos / Material", &_appState->_globalTriangleColor[0]);
		globalColorChanged |= ImGui::ColorEdit3("Nube de puntos", &_appState->_globalPointColor[0]);
		globalColorChanged |= ImGui::SliderFloat("Grosor linea", &_appState->_globalLineWidth, 0.1f, 10.0f);
		globalColorChanged |= ImGui::SliderFloat("Tamaño puntos", &_appState->_globalPointSize, 0.1f, 20.0f);

		if (globalColorChanged && sceneContent)
		{
			for (auto& model : sceneContent->_model)
			{
				model->setLineColor(_appState->_globalWireframeColor);
				model->setTriangleColor(_appState->_globalTriangleColor);
				model->setPointColor(_appState->_globalPointColor);
				model->setLineWidth(_appState->_globalLineWidth);
				model->setPointSize(_appState->_globalPointSize);
			}
		}

		GuiUtilities::leaveSpace(2);
		ImGui::Text("Topologia global");
		ImGui::Separator();

		static const char* topologyTitle[] = { "Point Cloud", "Wireframe", "Triangle Mesh" };
		for (int topologyIdx = 0; topologyIdx < VAO::NUM_IBOS; ++topologyIdx)
		{
			ImGui::Checkbox(topologyTitle[topologyIdx], &_appState->_activeRendering[topologyIdx]);
		}

		GuiUtilities::leaveSpace(2);
		ImGui::Text("Camara");
		ImGui::Separator();

		Camera* camera = sceneContent->_camera[_appState->_selectedCamera].get();
		if (camera)
		{
			float zNear = camera->getZNear();
			float zFar = camera->getZFar();
			if (ImGui::SliderFloat("Z Near", &zNear, 0.001f, 10.0f)) camera->setZNear(zNear);
			if (ImGui::SliderFloat("Z Far", &zFar, 10.0f, 5000.0f)) camera->setZFar(zFar);

			if (ImGui::Button("Reset Camara"))
			{
				camera->reset();
			}
		}

		GuiUtilities::leaveSpace(2);
		ImGui::Text("Controles de teclado");
		ImGui::Separator();
		ImGui::Text("WASD + Click Dcho: avanzar/retroceder/strafe");
		ImGui::Text("Q / E: subir / bajar");
		ImGui::Text("Scroll: zoom");
		ImGui::Text("T / R / S: gizmo translate/rotate/scale");
		ImGui::Text("0 / 1 / 2: toggle puntos/alambres/triangulos");
	}

	ImGui::End();
}

void AlgGeom::GUI::showScreenshotMenu(SceneContent* sceneContent)
{
	auto fixName = [=](const std::string& name, const std::string& defaultName, const std::string& extension) -> std::string
	{
		if (name.empty())
			return defaultName + extension;
		else if (name.find(extension) == std::string::npos)
			return name + extension;

		return name;
	};

	if (ImGui::Begin("Screenshot Settings", &_showMenuButtons[SCREENSHOT]))
	{
		ImGui::SliderFloat("Size multiplier", &_appState->_screenshotFactor, 1.0f, 10.0f);
		ImGui::SameLine(0, 20); ImGui::Checkbox("Transparent", &_appState->_transparentScreenshot);
		ImGui::InputText("Filename (RGB)", _appState->_screenshotFilenameBuffer, IM_ARRAYSIZE(_appState->_screenshotFilenameBuffer));

		GuiUtilities::leaveSpace(2);

		if (ImGui::Button("Take screenshot (RGBA)"))
		{
			std::string filename = _appState->_screenshotFilenameBuffer;
			filename = fixName(filename, "RGB", ".png");
			InputManager::getInstance()->pushScreenshotEvent(ScreenshotListener::ScreenshotEvent{ ScreenshotListener::RGBA });
		}
	}

	ImGui::End();
}
