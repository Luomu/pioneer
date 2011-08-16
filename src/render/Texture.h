#pragma once

#include <GL/glew.h>

namespace Render {
	/*
	 * 2D Texture holder
	 */
	class Texture {
	public:
		Texture(int w, int h);
		Texture(int w, int h, GLint format, GLint internalFormat, GLenum type);
		~Texture();
		virtual void Bind();
		virtual void Unbind();
		unsigned int GetGLTexture() const { return m_texture; }
		/*
		 * Draw the texture on screen, w/h in screen percent.
		 * This is only for testing.
		 */
		void Show(float x=0.f, float y=0.f, float w=100.f, float h=100.f);

	protected:
		/*
		 * Called just before glTexImage.
		 * Set filtering etc. here
		 */
		virtual void SetParameters();
		unsigned int m_texture;
		int m_w;
		int m_h;
		bool doMipmaps;
	};
}