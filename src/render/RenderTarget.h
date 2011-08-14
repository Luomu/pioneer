#pragma once
#include "Texture.h"

namespace Render {
	/*
	 * Render to a FBO texture
	 */
	class RenderTarget : public Texture {
	public:
		RenderTarget(int w, int h);
		RenderTarget(int w, int h, GLint format, GLint internalFormat,
			GLenum type);
		~RenderTarget();
		virtual void BeginRTT();
		virtual void EndRTT();

	protected:
		/*
		 * Throws an RenderException if FBO is incomplete.
		 * Must be bound before checking.
		 */
		virtual void CheckCompleteness() const;
		GLuint m_fbo;
		GLuint m_depth;
	};
}
