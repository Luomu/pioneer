#include "libs.h"
#include "Renderer.h"
#include "RenderTarget.h"
#include "RenderPostPass.h"
#include "RenderPostProgram.h"

namespace Render {

void Renderer::SwapBuffers()
{
	SDL_GL_SwapBuffers();
}

/* Generic colour + depth */
class PPSceneTarget : public RenderTarget {
public:
	PPSceneTarget(int width, int height, GLint format,
		GLint internalFormat, GLenum type) {
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
#define DEPTH_TEXTURE
#ifdef DEPTH_TEXTURE //had a problem with depth-to-texture on ATI, need to check
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		//glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
		m_depthTexture = new Texture(m_w, m_h, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_UNSIGNED_BYTE);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
			GL_TEXTURE_2D, m_depthTexture->GetGLTexture(), 0);
#else
		glGenRenderbuffersEXT(1, &m_depth);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depth);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_w, m_h);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
			GL_RENDERBUFFER_EXT, m_depth);
#endif
		CheckCompleteness();

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

#ifdef DEPTH_TEXTURE
	Texture *GetDepthTexture() const { return m_depthTexture; }

	~PPSceneTarget() {
		delete m_depthTexture;
	}
#else
	~PPSceneTarget() {
		glDeleteBuffers(1, &m_depth);
	}
#endif
private:
#ifdef DEPTH_TEXTURE
	Texture *m_depthTexture;
#else
	GLuint m_depth;
#endif
};

PostProcessingRenderer::PostProcessingRenderer(int width, int height) :
	Renderer(width, height)
{
	m_sceneTarget  = new PPSceneTarget(width, height, GL_RGB, GL_RGB, GL_FLOAT);
	desaturateProg = new Post::Program("ExampleFilter.vert", "ExampleFilterBlur.frag");
	compositeProg  = new Post::Program("ExampleFilter.vert", "ExampleComposite.frag");
	tempTarget1    = new RenderTarget(width, height, GL_RGB, GL_RGB, GL_FLOAT);
	SetUpPasses();
}

PostProcessingRenderer::~PostProcessingRenderer()
{
	delete m_sceneTarget;
	delete desaturateProg;
	delete compositeProg;
	delete tempTarget1;
	while (!m_passes.empty()) delete m_passes.back(), m_passes.pop_back();
}

void PostProcessingRenderer::BeginFrame()
{
	m_sceneTarget->BeginRTT();
}

void PostProcessingRenderer::EndFrame()
{
	m_sceneTarget->EndRTT();
	PostProcess();
}

void PostProcessingRenderer::SetUpPasses()
{
	using Post::Pass;
	Pass *p = new Pass(desaturateProg);
	p->AddSampler("fboTex", m_sceneTarget);
	p->renderToScreen = false;
	p->SetTarget(tempTarget1);
	m_passes.push_back(p);
	p = new Pass(compositeProg);
	p->AddSampler("first", tempTarget1);
	p->AddSampler("second", m_sceneTarget);
	p->renderToScreen = true;
	m_passes.push_back(p);
}

void PostProcessingRenderer::PostProcess()
{
	std::vector<Post::Pass*>::iterator it = m_passes.begin();
	for(; it != m_passes.end(); ++it) {
		(*it)->Execute();
	}
}

}
