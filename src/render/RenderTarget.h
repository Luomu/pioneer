#pragma once
#include "Texture.h"

namespace Render {
	/*
	 * Render to a FBO texture
	 */
	class RenderTarget : public Texture {
	public:
		RenderTarget(int w, int h, GLint format, GLint internalFormat,
			GLenum type);
		~RenderTarget();
		void BeginRTT();
		void EndRTT();

		/*
		 * Draw the texture on screen, w/h in screen percent.
		 * This is only for testing.
		 */
		void Show(float x=0.f, float y=0.f, float w=100.f, float h=100.f); //move this to Texture?
	private:
		GLuint m_fbo;
		GLuint m_depth;
	};
}
