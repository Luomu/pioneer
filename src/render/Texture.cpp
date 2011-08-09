#include "Texture.h"
#include <GL/glew.h>

namespace Render {

Texture::Texture(int w, int h, GLint format,
	GLint internalFormat, GLenum type)
{
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0,
		format, type, 0);
	SetParameters();
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_texture);
}

//Filtering, mipmapping etc.
//notice this texture is for rendertarget use
void Texture::SetParameters()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

}
