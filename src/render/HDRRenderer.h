#pragma once

#include <vector>
#include "Renderer.h"

namespace Render {

class RenderTarget;
class LuminanceRenderTarget;

namespace Post {
	class Filter;
}

/*
 * Renders to a HDR render target
 *
 * Replicates tomm's classic chain:
 * 1. Full screen 16f buffer (rectangle texture) + depth
 * 2. Fullscreen to 128x128 luminance buffer, generate mipmaps
 *    and extract average luminance from lowest mipmap level
 * 3. Bloom downsample & bright pass (half size buffer)
 * 4. Bloom downsample (quarter size buffer)
 * 5. Horizontally blur bloom texture
 * 6. Vertically blur previous texture
 * 7. Compose full screen texture and bloom texture,
 *    tonemap using average luminance and middle grey
 */
class HDRRenderer : public Renderer {
public:
	HDRRenderer(int w, int h);
	~HDRRenderer();
	void BeginFrame();
	void EndFrame();
	void ReloadShaders();

	void  SetLuminanceBias(float b);
private:
	RenderTarget *m_target;
	LuminanceRenderTarget *m_luminanceTarget;
	std::vector<Post::Filter*> m_filters;
};

}
