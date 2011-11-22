#ifndef _RENDERER_H
#define _RENDERER_H

namespace Render {

	class RenderTarget;
	class GLSLFilter;
	class ColorLUTFilter;

	class Renderer {
	public:
		Renderer(int w, int h) :
			m_width(w),
			m_height(h) { }
		virtual void BeginFrame() { };
		virtual void EndFrame() { };
		virtual void SwapBuffers();
	protected:
		int m_width;
		int m_height;
	};

	class PostProcessingRenderer : public Renderer {
	public:
		PostProcessingRenderer(int width, int height);
		virtual ~PostProcessingRenderer();
		virtual void BeginFrame();
		virtual void EndFrame();

	protected:
		void PostProcess();
		RenderTarget *m_sceneTarget;
		GLSLFilter *m_exampleFilter;
		GLSLFilter *m_exampleFilter2;
		ColorLUTFilter *m_exampleFilter3;
	};

}

#endif
