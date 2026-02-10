#pragma once

#include "FBO.h"
#include "Image.h"

namespace AlgGeom
{
	class FBOScreenshot : public FBO
	{
	protected:
		GLuint _multisampledFBO, _colorBufferID;
		GLuint _mColorBufferID, _mDepthBufferID;

	public:
		FBOScreenshot(const uint16_t width, const uint16_t height);
		~FBOScreenshot() override;

		GLuint getId() const override { return _multisampledFBO; }
		AlgGeom::Image* getImage() const;

		void bindFBO() override;
		void modifySize(const uint16_t width, const uint16_t height) override;
		void saveImage(const std::string& filename);
	};
}



