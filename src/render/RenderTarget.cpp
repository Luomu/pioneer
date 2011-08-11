#include "RenderTarget.h"
#include "Renderer.h"
#include "Texture.h"

namespace Render {

//todo: depth component optional, also consider MRTs
RenderTarget::RenderTarget(int w, int h, GLint format,
	GLint internalFormat, GLenum type) :
	Texture(w, h, format, internalFormat, type)
{
	glGenFramebuffers(1, &m_fbo);
	glGenRenderbuffers(1, &m_depth);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depth);

	glBindRenderbuffer(GL_RENDERBUFFER, m_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
		m_w, m_h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, m_depth);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, m_texture, 0);

	const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		throw RenderException("Incomplete FBO.");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget()
{
	glDeleteFramebuffers(1, &m_fbo);
	glDeleteRenderbuffers(1, &m_depth);
}

void RenderTarget::BeginRTT()
{
	//save current viewport and bind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, m_w, m_h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.f, 0.f, 0.f, 1.f);
}

void RenderTarget::EndRTT()
{
	//restore viewport and unbind fbo
	glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::Show(const float x, const float y,
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
