#include "Renderer.h"
#include "libs.h"
#include "RenderFilter.h"
#include "RenderTarget.h"
#include "RenderGLSLFilter.h"

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

		glGenRenderbuffersEXT(1, &m_depth);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depth);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, m_w, m_h);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
			GL_RENDERBUFFER_EXT, m_depth);

		CheckCompleteness();

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	~PPSceneTarget() {
		glDeleteRenderbuffersEXT(1, &m_depth);
	}
private:
	GLuint m_depth;
};

PostProcessingRenderer::PostProcessingRenderer(int width, int height) :
	Renderer(width, height)
{
	m_sceneTarget = new PPSceneTarget(width, height, GL_RGB, GL_RGB, GL_FLOAT);
	m_exampleFilter = new GLSLFilter("ExampleFilter.vert", "ExampleFilter.frag",
		new RenderTarget(width, height, GL_RGB, GL_RGB, GL_FLOAT));
	m_exampleFilter2 = new GLSLFilter("ExampleFilter.vert", "ExampleFilter.frag",
		new RenderTarget(width, height, GL_RGB, GL_RGB, GL_FLOAT));
	m_exampleFilter3 = new ColorLUTFilter(
		new RenderTarget(width, height, GL_RGB, GL_RGB, GL_FLOAT));
}

PostProcessingRenderer::~PostProcessingRenderer()
{
	delete m_sceneTarget;
	delete m_exampleFilter;
	delete m_exampleFilter2;
	delete m_exampleFilter3;
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

void PostProcessingRenderer::PostProcess()
{
	//scene has already been rendered to main rendertarget, grab texture
	Source acquireColourBuffer(m_sceneTarget); //basic Source op will do here now
	m_exampleFilter3->SetSource(&acquireColourBuffer);
	m_exampleFilter3->Execute();

	m_exampleFilter2->SetSource(m_exampleFilter3);
	m_exampleFilter2->Execute();

	m_exampleFilter->SetSource(m_exampleFilter2);
	m_exampleFilter->Execute();

	m_exampleFilter2->SetSource(m_exampleFilter);
	m_exampleFilter2->Execute();

	//present it on screen
	PresentOperator pop;
	pop.SetSource(m_exampleFilter2);
	pop.Execute();
}

}
