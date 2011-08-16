#pragma once

#include "gui/Gui.h"
#include "vector2.h"

namespace Render {

struct HdrParams {
	float key;
	float luminanceBias;
	HdrParams() :
		key(1.f),
		luminanceBias(5.f)
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
	static HdrParams& RenderParams();
private:
	Gui::TextEntry* AddParameter(const std::string&);
	Gui::TextEntry *m_exposureKey;
	Gui::TextEntry *m_luminanceBias;
	void OnChangeParameters();
	Gui::VBox *vbox;
};

}
