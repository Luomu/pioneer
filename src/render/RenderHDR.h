#ifndef _RENDERHDR_H
#define _RENDERHDR_H

#include "RenderTarget.h"
#include "RenderPostPass.h"

namespace Render {
	namespace Post {

	/*
	 * Classes for Pioneer's "classic" HDR implementation
	 */
	// calculates averageLuminance and middleGrey from scene texture
	class LuminanceTarget : public RenderTarget {
	public:
		LuminanceTarget(const TextureFormat &format);
		// calls generateMipmaps + extracts values
		virtual void EndRTT();
		float averageLuminance;
		float middleGrey;
	private:
		TextureFormat m_format;
	};

	// extract average luminance from LuminanceTarget
	class AverageLuminance : public Uniform {
	public:
		AverageLuminance(const std::string &name, RefCountedPtr<LuminanceTarget> t);
		virtual void Set();
		RefCountedPtr<LuminanceTarget> luminanceTexture;
	};

	// extract middle grey from LuminanceTarget
	class MiddleGrey : public AverageLuminance {
	public:
		MiddleGrey(const std::string &name, RefCountedPtr<LuminanceTarget> t);
		virtual void Set();
	};

	}
}
#endif
