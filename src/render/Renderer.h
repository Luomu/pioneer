#pragma once

#include <stdexcept>

/*
 * Controls render targets and post processing
 */
class Renderer {
public:
	void BeginFrame();
	void EndFrame();
	void SwapBuffers();
};

class RenderException : public std::runtime_error
{
public:
	RenderException(const std::string &what) : std::runtime_error(what.c_str()) { }
	RenderException(const char *what) : std::runtime_error(what) { }
};
