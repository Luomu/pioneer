#pragma once

#include "gui/Gui.h"
#include "vector2.h"

namespace Render {

/*
 * Some knobs and sliders for adjusting the image
 */
class Controls : public Gui::Fixed
{
public:
	Controls();
	vec2i m_requestedSize;
};

}
