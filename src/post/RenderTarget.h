#pragma once

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
		GLuint GetTexture() const { return m_texture; }
		void Show(float x=0.f, float y=0.f, float w=100.f, float h=100.f) const;
	private:
		int m_w;
		int m_h;
		GLuint m_fbo;
		GLuint m_depth;
		GLuint m_texture;
	};
}