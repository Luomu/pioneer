#ifndef _RENDERTARGET_H
#define _RENDERTARGET_H
#include "RenderTexture.h"
#include "RefCounted.h"

namespace Render {

	struct TextureFormat {
		TextureFormat() :
			format(GL_RGB), internalFormat(GL_RGB8), type(GL_UNSIGNED_BYTE)
		{ }
		TextureFormat(GLint fo, GLint ifo, GLenum typ) :
			format(fo), internalFormat(ifo), type(typ)
		{ }
		GLint format;
		GLint internalFormat;
		GLenum type;
	};

	/*
	 * FBO rendertarget, only color
	 */
	class RenderTarget : public Texture, public RefCounted {
	public:
		RenderTarget();
		RenderTarget(int w, int h, GLint format,
			GLint internalFormat, GLenum type);
		virtual ~RenderTarget();
		virtual void BeginRTT();
		virtual void EndRTT();

		//more readable this way with rendertargets
		inline void BindTexture() { Bind(); }
		inline void UnbindTexture() { Unbind(); }

	protected:
		/*
		 * Throws an exception if FBO is incomplete.
		 * Bind before checking.
		 */
		virtual void CheckCompleteness() const;
		GLuint m_fbo;
	};
}

#endif
