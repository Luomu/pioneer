#include "Renderer.h"
#include <SDL.h>

namespace Render {

Renderer::Renderer(int w, int h) :
	m_width(w),
	m_height(h)
{

}

void Renderer::SwapBuffers()
{
	SDL_GL_SwapBuffers();
}

}
