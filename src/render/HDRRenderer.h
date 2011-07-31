#pragma once

#include "Renderer.h"

namespace Render {

class RenderTarget;

namespace Post { class Filter; }

/*
 * Renders to a HDR render target
 */
class HDRRenderer : public Renderer {
public:
	HDRRenderer(int w, int h);
	~HDRRenderer();
	void BeginFrame();
	void EndFrame();
	void SwapBuffers();
private:
	RenderTarget *m_target;
	RenderTarget *m_target2;
	Post::Filter *m_blurFilter;
};

}