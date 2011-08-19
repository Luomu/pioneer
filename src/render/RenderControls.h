#pragma once

#include "gui/Gui.h"
#include "vector2.h"
#include <map>

namespace Render {

/*
 * Bunch of HDR parameters. All parameters do not make
 * sense with all tone mapping operators.
 */
struct HdrParams {
	float key;
	float luminanceBias;
	float luminanceSaturation;
	float whiteLevel;
	HdrParams() :
		key(1.f),
		luminanceBias(5.f),
		luminanceSaturation(1.f),
		whiteLevel(1.f)
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
	std::map<std::string, Gui::TextEntry*> m_entries;
	Gui::TextEntry* AddParameter(const std::string&, float);
	void OnChangeParameters();
	float GetFloat(const std::string&);
	Gui::VBox *vbox;
};

}
