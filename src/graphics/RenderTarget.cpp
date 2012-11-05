#include "RenderTarget.h"

namespace Graphics {

GLTexture::GLTexture(unsigned int w, unsigned int h) {
	const GLenum target = GL_TEXTURE_2D;
	glGenTextures(1, &texture);
	glBindTexture(target, texture);
	glTexImage2D(target, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(target, 0);
}

void GLTexture::Bind() {
	glBindTexture(GL_TEXTURE_2D, texture);
}

void GLTexture::Unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLTexture::~GLTexture() {
	glDeleteTextures(1, &texture);
}

GLFrameBuffer::GLFrameBuffer() {
	glGenFramebuffers(1, &buffer);
}

GLFrameBuffer::~GLFrameBuffer() {
	glDeleteFramebuffers(1, &buffer);
}

void GLFrameBuffer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, buffer);
}

void GLFrameBuffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLRenderBuffer::GLRenderBuffer() {
	glGenRenderbuffers(1, &buffer);
}

void GLRenderBuffer::Bind() {
	glBindRenderbuffer(GL_RENDERBUFFER, buffer);
}

GLRenderBuffer::~GLRenderBuffer() {
	glDeleteRenderbuffers(1, &buffer);
}

RenderTarget::RenderTarget(unsigned int w, unsigned int h)
: m_width(w)
, m_height(h)
{
	m_fbo.Reset(new GLFrameBuffer());
	m_fbo->Bind();

	m_depthBuffer.Reset(new GLRenderBuffer());
	m_depthBuffer->Bind();
	glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, w, h);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer->buffer);

	m_texture.Reset(new GLTexture(w, h));
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->texture, 0);

	const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		throw std::string("Incomplete framebuffer");

	m_fbo->Unbind();
}

void RenderTarget::BeginRTT() {
	m_fbo->Bind();
	glViewport(0, 0, m_width, m_height);
}

void RenderTarget::EndRTT() {
	m_fbo->Unbind();
}

GLTexture *RenderTarget::GetTexture() const {
	return m_texture.Get();
}

}