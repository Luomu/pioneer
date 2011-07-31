#include "Renderer.h"

void Renderer::BeginFrame()
{
	//bind fbo
}

void Renderer::EndFrame()
{
	//unbind fbo and do post
}

void Renderer::SwapBuffers()
{
	SDL_GL_SwapBuffers();
}
