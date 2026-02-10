#pragma once

#include "Camera.h"
#include "InterfaceAdapter.h"

namespace AlgGeom
{
	class CameraGuiAdapter : public InterfaceAdapter
	{
	private:
		Camera* _camera;

	public:
		CameraGuiAdapter() : _camera(nullptr) {}
		virtual ~CameraGuiAdapter() {}

		void renderGuiObject() override;
		void setCamera(Camera* camera) { _camera = camera; }
	};
}


