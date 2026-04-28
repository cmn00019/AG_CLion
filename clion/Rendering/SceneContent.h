#pragma once

#include "stdafx.h"
#include "AABB.h"
#include "ApplicationState.h"
#include "Camera.h"
#include "Model3D.h"

class TriangleModel;
class Octree;
class PointCloud3d;
#include "Triangle3d.h"
#include "PointCloud3d.h"

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
		
		// PR5 variables
		bool _isPr5Active = false;
		bool _showPr5Slow = false;
		bool _pr5Wireframe = false;
		int _pr5SpeedMs = 500;
		::TriangleModel* _chSlowModel = nullptr;
		mat4 _pr5ModelMatrix = mat4(1.0f);
		int _visibleSlow = 0;
		int _frameCounterPr5 = 0;
		PointCloud3d* _cloudPr5 = nullptr;
		int _numPointsPr5 = 500;

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
		void buildDelaunay();
		void generateCloudPr5();
		void extractCloudFromModel();
		void runCH_Lento();
		void runCH_Opt();
		void clearPr5Scene();
		void update_pr5();
		void update_pr4(bool skipTriTest = false);
		void syncPr4Visuals();
		void clearPr4Visuals();
		void runPr4BruteForce();
		Model3D* getModel(Model3D::Component* component);

	};
}