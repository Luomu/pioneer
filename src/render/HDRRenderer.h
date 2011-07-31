#pragma once

#include "Renderer.h"

namespace Render {

/*
 * Renders to a HDR render target
 */
class HDRRenderer : public Renderer {
public:
	HDRRenderer();
	void BeginFrame();
	void EndFrame();
	void SwapBuffers();
private:
	RenderTarget *m_target;
};

}