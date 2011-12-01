#ifndef _RENDERER_H
#define _RENDERER_H

#include <vector>

namespace Render {

	class RenderTarget;

	namespace Post {
		class Program;
		class Pass;
	}

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
		void SetUpPasses();
		RenderTarget *m_sceneTarget;
		RenderTarget *tempTarget1;
		Post::Program *desaturateProg;
		Post::Program *compositeProg;
		std::vector<Post::Pass *> m_passes;
	};

}

#endif
