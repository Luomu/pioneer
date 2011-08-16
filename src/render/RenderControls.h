#pragma once

#include "gui/Gui.h"
#include "vector2.h"

namespace Render {

struct HdrParams {
	float luminanceBias;
	HdrParams() :
		luminanceBias(0.5f)
	{ }
};

/*
 * Some knobs and sliders for adjusting the image
 */
class Controls : public Gui::Fixed
{
public:
	Controls();
	void GetSizeRequested(float size[2]);
	vec2i m_requestedSize;
	static HdrParams* RenderParams();
private:
	Gui::TextEntry *m_luminanceSaturation;
	void OnChangeParameters();
};

}
