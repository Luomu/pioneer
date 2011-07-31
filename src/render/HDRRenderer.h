#pragma once

#include "Renderer.h"

namespace Render {

class RenderTarget;

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
};

}