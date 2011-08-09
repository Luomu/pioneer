#pragma once

#include <GL/glew.h>

namespace Render {
	/*
	 * 2D Texture holder
	 */
	class Texture {
	public:
		Texture(int w, int h, GLint format, GLint internalFormat, GLenum type);
		~Texture();
		virtual void Bind();
		virtual void Unbind();
		unsigned int GetGLTexture() const { return m_texture; }
	protected:
		void SetParameters();
	private:
		unsigned int m_texture;
	};
}
