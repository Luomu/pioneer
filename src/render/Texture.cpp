#include "Texture.h"
#include <GL/glew.h>

namespace Render {

Texture::Texture(int w, int h) :
	m_w(w),
	m_h(h),
	m_texture(0)
{

}

Texture::Texture(int w, int h, GLint format,
	GLint internalFormat, GLenum type) :
	m_w(w),
	m_h(h),
	doMipmaps(false)
{
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	SetParameters();
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0,
		format, type, 0);
	if(doMipmaps)
		glGenerateMipmap(GL_TEXTURE_2D);
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

void Texture::Show(const float x, const float y,
	const float w, const float h)
{
	const int width = 100;
	const int height = 100;
	glColor4f(1.f, 1.f, 1.f, 1.f);

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	Texture::Bind();
	glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, 100.f);

	glLoadIdentity();

	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 1.f);
	 glVertex2f(x, y);
	glTexCoord2f(0.f, 0.f);
	 glVertex2f(x, y + h);
	glTexCoord2f(1.f, 0.f);
	 glVertex2f(x + w, y + h);
	glTexCoord2f(1.f, 1.f);
	 glVertex2f(x + w, y);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopAttrib();
}

}