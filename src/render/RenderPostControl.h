#ifndef _RENDERPOSTCONTROL_H
#define _RENDERPOSTCONTROL_H

#include <vector>
#include "render/RenderTarget.h"
#include "RenderPostSceneTarget.h"
#include "RefCounted.h"

namespace Render {

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
		class Control : public RefCounted {
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
			void SetUpClassicHDR();

			// create & add a pass from program
			Pass* AddPass(RefCountedPtr<Program> prog);

			SceneTarget *m_sceneTarget;
			std::vector<Post::Pass *> m_passes;

			// current viewport dimensions (can change during passes)
			int m_viewWidth;
			int m_viewHeight;
			TextureFormat m_tf;
			TextureFormat m_hdrtf;
		};
	}
}

#endif
