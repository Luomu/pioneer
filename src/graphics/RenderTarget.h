#pragma once

#include "libs.h"
#include "TextureGL.h"
// Render to texture hack for UI
namespace Graphics {

struct GLTexture : public TextureGL {
	GLTexture(unsigned int w, unsigned int h);
	virtual ~GLTexture();
	virtual void Bind();
	virtual void Unbind();
	GLuint texture;
};

struct GLFrameBuffer {
	GLFrameBuffer();
	void Bind();
	void Unbind();
	virtual ~GLFrameBuffer();
	GLuint buffer;
};

struct GLRenderBuffer {
	GLRenderBuffer();
	virtual ~GLRenderBuffer();
	void Bind();
	GLuint buffer;
};

class RenderTarget {
public:
	RenderTarget(unsigned int w, unsigned int h);

	void BeginRTT();
	void EndRTT();

	GLTexture *GetTexture() const;

private:
	ScopedPtr<GLFrameBuffer> m_fbo;
	ScopedPtr<GLRenderBuffer> m_depthBuffer;
	ScopedPtr<GLTexture> m_texture;

	int m_width;
	int m_height;
};

}
