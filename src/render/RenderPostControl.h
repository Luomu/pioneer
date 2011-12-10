#ifndef _RENDERPOSTCONTROL_H
#define _RENDERPOSTCONTROL_H

#include <vector>
#include "render/RenderTarget.h"

namespace Render {

	/*
	 * Rendertarget with a depth texture attachment
	 */
	class PostSceneTarget : public RenderTarget {
	public:
		PostSceneTarget(int width, int height);
		virtual ~PostSceneTarget();
		Texture *GetDepthTexture() const { return m_depthTexture; }
	private:
		Texture *m_depthTexture;
	};

	namespace Post {

		class Pass;
		class Program;

		/*
		 * Post-processing control
		 * Holds the filter (pass) chain and executes each pass
		 *
		 * Design notes:
		 * rendertargets should be shared as much as possible
		 * same for glsl progs
		 */
		class Control {
		public:
			Control(int width, int height);
			virtual ~Control();
			void BeginFrame();
			void EndFrame();

			// XXX somebody implement vec2 please
			int GetViewportWidth()  const { return m_viewWidth; }
			int GetViewportHeight() const { return m_viewHeight; }
		protected:
			void PostProcess();
			virtual void SetUpPasses();

			PostSceneTarget *m_sceneTarget;
			std::vector<Post::Pass *> m_passes;

			// current viewport dimensions (can change during passes)
			int m_viewWidth;
			int m_viewHeight;
		};
	}
}

#endif
