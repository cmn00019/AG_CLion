#pragma once

#include "stdafx.h"
#include "AABB.h"
#include "ApplicationState.h"
#include "Camera.h"
#include "Model3D.h"

class TriangleModel;
class Octree;

namespace AlgGeom
{
	class SceneContent
	{
	public:
		std::vector<std::unique_ptr<Camera>>	_camera;
		std::vector<std::unique_ptr<Model3D>>	_model;
		AABB									_sceneAABB;
		std::string								_currentModelPath;
		std::string								_currentModelPathB;
		bool									_isPr4Active = false;
		bool									_showYellowBoxes = true;
		bool									_showRedTriangles = true;
		bool									_isBruteForceActive = false;
		
		::TriangleModel*						_tmA = nullptr;
		::TriangleModel*						_tmB = nullptr;
		::Octree*								_octA = nullptr;
		::Octree*								_octB = nullptr;
		Model3D*								_drawA_ref = nullptr;
		Model3D*								_drawB_ref = nullptr;
		Model3D*                                _drawOctA_ref = nullptr;
		Model3D*                                _drawOctB_ref = nullptr;
		Model3D*                                _pr4_reds = nullptr;
		Model3D*                                _pr4_redsB = nullptr;
		Model3D*                                _pr4_boxesA = nullptr;
		Model3D*                                _pr4_boxesB = nullptr;

		SceneContent();
		virtual ~SceneContent();

		void addNewCamera(ApplicationState* appState);
		void addNewModel(Model3D* model);
		void clearScene();
		void buildScenario();
		void buildPr1a();
		void buildPr1b();
		void buildPr2a();
		void buildPr2b();
		void buildPr2c();
		void buildPr2d();
		void buildPr3a();
		void buildPr4();
		void update_pr4(bool skipTriTest = false);
		void syncPr4Visuals();
		void clearPr4Visuals();
		void runPr4BruteForce();
		Model3D* getModel(Model3D::Component* component);

	};
}