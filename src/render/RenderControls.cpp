#include "RenderControls.h"

namespace Render {
	
Controls::Controls() :
	m_requestedSize(100, 100)
{
	Gui::VBox *vbox = new Gui::VBox();
	Add(vbox, 0, 0);

	vbox->PackEnd(new Gui::Label("Luminance saturation:"));
	m_luminanceSaturation = new Gui::TextEntry();
	vbox->PackEnd(m_luminanceSaturation);

	Gui::LabelButton *b = new Gui::LabelButton(new Gui::Label("Apply"));
	b->onClick.connect(sigc::mem_fun(this, &Render::Controls::OnChangeParameters));
	vbox->PackEnd(b);
}

void Controls::GetSizeRequested(float size[2])
{
	size[0] = m_requestedSize.x;
	size[1] = m_requestedSize.y;
}

void Controls::OnChangeParameters()
{
	const float lumb = atof(m_luminanceSaturation->GetText().c_str());
	RenderParams()->luminanceBias = lumb;
}

HdrParams* Controls::RenderParams()
{
	static HdrParams *params = new HdrParams;
	return params;
}

} //namespace Render
