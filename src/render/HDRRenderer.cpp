#include "HDRRenderer.h"
#include "RenderTarget.h"
#include "PostFilter.h"

namespace Render {

class HDRRenderTarget : public RenderTarget {
public:
	HDRRenderTarget(int w, int h) :
		RenderTarget(w, h, GL_RGB, GL_RGB16F, GL_HALF_FLOAT)
	{ }
};

HDRRenderer::HDRRenderer(int w, int h) :
	Renderer(w, h)
{
	//using FBOs like this is questionable. It would be better to shuffle attachments
	//or just flip textures assuming the dimensions/formats are the same
	m_target = new HDRRenderTarget(w, h);
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
	Post::Present present(m_target);
	present.Execute();
	//~ m_target->Show(0.f, 0.f, 30.f, 30.f);
}

}
