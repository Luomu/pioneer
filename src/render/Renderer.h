#pragma once

#include <stdexcept>

namespace Render {

class RenderTarget;

/*
 * Controls render targets and post processing
 */
class Renderer {
public:
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
	void SwapBuffers();
};

class RenderException : public std::runtime_error
{
public:
	RenderException(const std::string &what) : std::runtime_error(what.c_str()) { }
	RenderException(const char *what) : std::runtime_error(what) { }
};

}