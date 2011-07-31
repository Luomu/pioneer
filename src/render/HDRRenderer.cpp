#include "HDRRenderer.h"
#include "RenderTarget.h"
#include "PostFilter.h"

namespace Render {

HDRRenderer::HDRRenderer(int w, int h) :
	Renderer(w, h)
{
	//using FBOs like this is questionable. It would be better to shuffle attachments
	//or just flip textures assuming the dimensions/formats are the same
	m_target = new RenderTarget(w, h);
	m_target2 = new RenderTarget(w, h);

	m_blurFilter = new Post::Blur(m_target2, m_target);
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
	//todo: no need to construct the filter chain on every frame
	m_target->EndRTT();
	//m_target->Show(0.f, 0.f, 100.f, 100.f);
	Post::Tint tint(m_target, m_target2);
	tint.Execute();
	m_blurFilter->Execute();
	Post::Present present(m_target);
	present.Execute();
}

}