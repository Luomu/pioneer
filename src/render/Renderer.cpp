#include "Renderer.h"

Renderer::BeginFrame()
{
	//bind fbo
}

Renderer::EndFrame()
{
	//unbind fbo and do post
}

Renderer::SwapBuffers()
{
	SDL_GL_SwapBuffers();
}
