#include "Renderer.h"
#include "libs.h"
#include "RenderFilter.h"
#include "RenderTarget.h"

namespace Render {

void Renderer::SwapBuffers()
{
	SDL_GL_SwapBuffers();
}

PostProcessingRenderer::PostProcessingRenderer()
{
	m_sceneTarget = new RenderTarget(1024, 768, GL_RGB, GL_RGB, GL_FLOAT);
}

PostProcessingRenderer::~PostProcessingRenderer()
{
	delete m_sceneTarget;
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
	//simple passthrough

	//scene has already been rendered to main rendertarget, grab texture
	Source acquireColourBuffer(m_sceneTarget); //basic Source op will do here now

	//present it on screen
	PresentOperator pop;
	pop.SetSource(&acquireColourBuffer);
	pop.Execute();
}

}
