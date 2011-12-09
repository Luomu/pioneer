#include "RenderPostControl.h"
#include "RenderPostPass.h"
#include "RenderPostProgram.h"

namespace Render {

PostSceneTarget::PostSceneTarget(int width, int height)
{
	const GLint format         = GL_RGB;
	const GLint internalFormat = GL_RGB;
	const GLenum type          = GL_FLOAT;
	m_w = width;
	m_h = height;

	glGenFramebuffersEXT(1, &m_fbo);
	glGenTextures(1, &m_texture);
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_fbo);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_w, m_h, 0,
		format, type, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
		GL_TEXTURE_2D, m_texture, 0);

	m_depthTexture = new Texture(m_w, m_h, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_UNSIGNED_BYTE);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
		GL_TEXTURE_2D, m_depthTexture->GetGLTexture(), 0);

	CheckCompleteness();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

PostSceneTarget::~PostSceneTarget()
{
	delete m_depthTexture;
}

namespace Post {

Control::Control(int width, int height)
{
	// request a SceneTarget of appropriate size from some manager
	// request programs and the rest of targets from some manager

	m_sceneTarget = new PostSceneTarget(width, height);
	SetUpPasses();
}

Control::~Control()
{
	while (!m_passes.empty()) delete m_passes.back(), m_passes.pop_back();
	delete dummyProg;
}

void Control::BeginFrame()
{
	m_sceneTarget->BeginRTT();
}

void Control::EndFrame()
{
	m_sceneTarget->EndRTT();
	PostProcess();
}

void Control::PostProcess()
{
	std::vector<Post::Pass*>::iterator it = m_passes.begin();
	for(; it != m_passes.end(); ++it) {
		(*it)->Execute();
	}
}

void Control::SetUpPasses()
{
	dummyProg         = new Post::Program("filters/Quad.vert", "filters/Passthrough.frag");
	Post::Pass *p     = new Pass(dummyProg);
	p->AddSampler("texture0", m_sceneTarget);
	p->renderToScreen = true;
	m_passes.push_back(p);
}

} }
