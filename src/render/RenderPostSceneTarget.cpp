#include "RenderPostSceneTarget.h"

namespace Render { namespace Post {

SceneTarget::SceneTarget(int width, int height, const TextureFormat &tf)
{
	const GLint format         = tf.format;
	const GLint internalFormat = tf.internalFormat;
	const GLenum type          = tf.type;
	m_w = width;
	m_h = height;

	// XXX this is always multisampled, that makes it possible to enable
	// and disable multisampling without recreating the target
	GLint samples = 0;
	glGetIntegerv(GL_SAMPLES, &samples);

	// multisampled fbo
	glGenFramebuffersEXT(1, &m_msFbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_msFbo);
	// ms color buffer
	glGenRenderbuffersEXT(1, &m_msColor);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_msColor);
	glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, samples, internalFormat, m_w, m_h);
	// ms depth buffer
	glGenRenderbuffersEXT(1, &m_depth);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depth);
	glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, samples, GL_DEPTH_COMPONENT24, m_w, m_h);
	// attach
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, m_msColor);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  GL_RENDERBUFFER_EXT, m_depth);

	CheckCompleteness();

	glGenFramebuffersEXT(1, &m_fbo);
	glGenTextures(1, &m_texture);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
	Bind();
	SetWrapMode(CLAMP);
	SetFilterMode(LINEAR);
	glTexImage2D(m_target, 0, internalFormat, m_w, m_h, 0,
		format, type, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
		m_target, m_texture, 0);

	CheckCompleteness();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

SceneTarget::~SceneTarget()
{
	//delete m_depthTexture;
	glDeleteRenderbuffersEXT(1, &m_msColor);
	glDeleteRenderbuffersEXT(1, &m_depth);
	glDeleteFramebuffersEXT(1, &m_msFbo);
}

void SceneTarget::BeginRTT()
{
	//begin rendering to multisampled FBO
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_msFbo);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, m_w, m_h);
}

void SceneTarget::EndRTT()
{
	//blit multisampled rendering to normal fbo
	glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, m_msFbo);
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, m_fbo);
	//depth testing has already been done, so color is enough
	//sizes are 1:1 so NEAREST is appropriate
	glBlitFramebufferEXT(0, 0, m_w, m_h, 0, 0, m_w, m_h, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
}

} }
