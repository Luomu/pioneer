#include "RenderTexture.h"

namespace Render {

//Don't init a texture, one can be generated or loaded later
Texture::Texture() :
	m_w(0),
	m_h(0),
	m_texture(0),
	m_filterMode(LINEAR),
	m_wrapMode(CLAMP),
	m_target(GL_TEXTURE_2D)
{ }

Texture::Texture(int w, int h, GLint format,
	GLint internalFormat, GLenum type) :
	m_w(w),
	m_h(h),
	m_target(GL_TEXTURE_2D)
{
	glGenTextures(1, &m_texture);

	Bind();

	SetFilterMode(LINEAR);
	SetWrapMode(CLAMP);

	glTexImage2D(m_target, 0, internalFormat, w, h, 0,
		format, type, 0);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_texture);
}

void Texture::Bind()
{
	glBindTexture(m_target, m_texture);
}

void Texture::Unbind()
{
	glBindTexture(m_target, 0);
}

void Texture::SetFilterMode(const FilterMode mode)
{
	m_filterMode = mode;
	GLint fmode = GL_LINEAR;
	if (mode == NEAREST)
		fmode = GL_NEAREST;
	glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, fmode);
	glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, fmode);
}

void Texture::SetWrapMode(const WrapMode mode)
{
	m_wrapMode = mode;
	GLint wmode = GL_CLAMP_TO_EDGE;
	if (mode == REPEAT)
		wmode = GL_REPEAT;
	glTexParameteri(m_target, GL_TEXTURE_WRAP_S, wmode);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_T, wmode);
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
	glEnable(m_target);
	Texture::Bind();

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

void Texture::Load(const std::string &filename)
{
	if (m_texture > 0)
		glDeleteTextures(1, &m_texture);

	SDL_Surface *s = IMG_Load(filename.c_str());
	glGenTextures (1, &m_texture);
	Bind();

	SetFilterMode(m_filterMode);
	SetWrapMode(m_wrapMode);

	m_w = s->w;
	m_h = s->h;
	glTexImage2D(m_target, 0, GL_RGBA, m_w, m_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
	Unbind();
	SDL_FreeSurface(s);
}

}
