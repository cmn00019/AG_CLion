#pragma once

#include "stdafx.h"
#include "Texture.h"
#include "VAO.h"


namespace AlgGeom
{
	struct ApplicationState
	{
		// Application
		vec3							_backgroundColor;								
		float							_materialScattering;								
		uint16_t						_numFps;											
		uint8_t							_selectedCamera;								
		ivec2							_viewportSize;										

		// Screenshot
		char							_screenshotFilenameBuffer[60];				
		float							_screenshotFactor;	
		bool							_transparentScreenshot;

		// Ligthing
		vec3							_lightPosition, _Ia, _Id, _Is;	
		float							_gamma;

		// Topology
		bool							_activeRendering[VAO::NUM_IBOS];

		// Global rendering colors
		vec3							_globalWireframeColor;
		vec4							_globalTriangleColor;
		vec3							_globalPointColor;
		float							_globalLineWidth;
		float							_globalPointSize;

		ApplicationState()
		{
			_backgroundColor = vec3(0.15f);
			_materialScattering = 1.0f;
			_numFps = 0;
			_selectedCamera = 0;
			_viewportSize = vec3(0);

			std::strcpy(_screenshotFilenameBuffer, "ScreenshotRGBA.png");
			_screenshotFactor = 3.0f;

			for (int i = 0; i < VAO::NUM_IBOS; ++i) _activeRendering[i] = /*i == VAO::IBO_TRIANGLE*/true;

			_globalWireframeColor = vec3(1.0f);
			_globalTriangleColor = vec4(1.0f, 0.5f, 0.0f, 1.0f);
			_globalPointColor = vec3(1.0f);
			_globalLineWidth = 1.0f;
			_globalPointSize = 3.0f;

			_lightPosition = vec3(.0f);
			_Ia = vec3(.6f);
			_Id = vec3(1.0f);
			_Is = vec3(1.0f);
			_gamma = 1.3f;
		}
	};
}