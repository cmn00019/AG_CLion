#pragma once

#include "stdafx.h"
#include "AABB.h"
#include "ApplicationState.h"
#include "Camera.h"
#include "Model3D.h"

namespace AlgGeom
{
	class SceneContent
	{
	public:
		std::vector<std::unique_ptr<Camera>>	_camera;
		std::vector<std::unique_ptr<Model3D>>	_model;
		AABB									_sceneAABB;

		SceneContent();
		virtual ~SceneContent();

		void addNewCamera(ApplicationState* appState);
		void addNewModel(Model3D* model);
		void clearScene();
		void buildScenario();
		void buildPr1a();
		void buildPr1b();
		Model3D* getModel(Model3D::Component* component);
	};
}