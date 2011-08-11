#pragma once

#include <stdexcept>

namespace Render {

/*
 * Controls render targets and post processing
 */
class Renderer {
public:
	Renderer(int w, int h);
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void ReloadShaders() { }
	void SwapBuffers();
protected:
	int m_width;
	int m_height;
};

class RenderException : public std::runtime_error
{
public:
	RenderException(const std::string &what) : std::runtime_error(what.c_str()) { }
	RenderException(const char *what) : std::runtime_error(what) { }
};

}