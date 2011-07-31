#include "HDRRenderer.h"
#include "RenderTarget.h"
#include "PostFilter.h"

namespace Render {

HDRRenderer::HDRRenderer(int w, int h) :
	Renderer(w, h)
{
	m_target = new RenderTarget(w, h);
	m_exampleFilter = new Post::Present(m_target);
}

HDRRenderer::~HDRRenderer()
{
	delete m_target;
	delete m_exampleFilter;
}

void HDRRenderer::BeginFrame()
{
	m_target->BeginRTT();
}

void HDRRenderer::EndFrame()
{
	m_target->EndRTT();
	//m_target->Show(0.f, 0.f, 100.f, 100.f);
	m_exampleFilter->Execute();
}

}