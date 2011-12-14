#ifndef _RENDERPOSTSCENETARGET_H
#define _RENDERPOSTSCENETARGET_H

#include "RenderTarget.h"

namespace Render {
	namespace Post {
		/*
		 * Rendertarget with a depth attachment, for capturing the scene.
		 * Can be multisampled.
		 */
		class SceneTarget : public RenderTarget {
		public:
			SceneTarget(int width, int height, const TextureFormat &f);
			virtual ~SceneTarget();
			//Texture *GetDepthTexture() const { return m_depthTexture; }

			virtual void BeginRTT();
			virtual void EndRTT();
		private:
			//XXX access to depthTexture can be useful in shaders - but it
			//needs to be extracted from the ms'd depth buffer
			//Texture *m_depthTexture;
			GLuint m_msFbo;
			GLuint m_msColor;
			GLuint m_depth;
		};
	}
};

#endif
