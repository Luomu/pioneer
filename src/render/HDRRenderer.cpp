#include "HDRRenderer.h"
#include "RenderTarget.h"

namespace Render {

HDRRenderer::HDRRenderer(int w, int h) :
	Renderer(w, h)
{
	m_target = new RenderTarget(w, h);
}

HDRRenderer::~HDRRenderer()
{
	delete m_target;
}

void HDRRenderer::BeginFrame()
{
	m_target->BeginRTT();
}

void HDRRenderer::EndFrame()
{
	m_target->EndRTT();
	m_target->Show(0.f, 0.f, 100.f, 100.f);
}

}