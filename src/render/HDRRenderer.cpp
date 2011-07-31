#include "HDRRenderer.h"

namespace Render {

HDRRenderer::HDRRenderer()
{

}

void HDRRenderer::BeginFrame()
{
	//bind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HDRRenderer::EndFrame()
{
	//unbind fbo and do post
}

}