#pragma once

#include <stdexcept>

/*
 * Render to a FBO texture
 */
namespace Post {
	class RenderTarget {
	public:
		RenderTarget(int w, int h);
		~RenderTarget();
		void BeginRTT();
		void EndRTT();
	private:
		int m_w;
		int m_h;
		GLuint m_fbo;
		GLuint m_depth;
		GLuint m_texture;
	};
}

class RenderException : public std::runtime_error
{
public:
	RenderException(const std::string &what) : std::runtime_error(what.c_str()) { }
	RenderException(const char *what) : std::runtime_error(what) { }
};