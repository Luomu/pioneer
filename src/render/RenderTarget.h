#pragma once
#include <GL/glew.h>

namespace Render {
	class Texture;

	/*
	 * Render to a FBO texture
	 */
	class RenderTarget {
	public:
		RenderTarget(int w, int h);
		~RenderTarget();
		void BeginRTT();
		void EndRTT();
		Texture* GetTexture() const;
		void Show(float x=0.f, float y=0.f, float w=100.f, float h=100.f) const;
	protected:
		RenderTarget(int w, int h, GLint format, GLint internalFormat,
			GLenum type);
	private:
		int m_w;
		int m_h;
		GLuint m_fbo;
		GLuint m_depth;
		Texture *m_texture;
	};
}
