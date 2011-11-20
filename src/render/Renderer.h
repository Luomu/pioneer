#ifndef _RENDERER_H
#define _RENDERER_H

namespace Render {

	class RenderTarget;

	class Renderer {
	public:
		virtual void BeginFrame() { };
		virtual void EndFrame() { };
		virtual void SwapBuffers();
	};

	class PostProcessingRenderer : public Renderer {
	public:
		PostProcessingRenderer();
		virtual ~PostProcessingRenderer();
		virtual void BeginFrame();
		virtual void EndFrame();

	protected:
		void PostProcess();
		RenderTarget *m_sceneTarget;
	};

}

#endif
