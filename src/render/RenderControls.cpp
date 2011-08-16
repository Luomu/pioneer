#include "RenderControls.h"

namespace Render {
	
Controls::Controls() :
	m_requestedSize(100, 1000)
{
	vbox = new Gui::VBox();
	Add(vbox, 0, 0);

	m_luminanceBias       = AddParameter("Luminance bias");
	m_exposureKey         = AddParameter("Key value");

	Gui::LabelButton *b = new Gui::LabelButton(new Gui::Label("Apply"));
	b->onClick.connect(sigc::mem_fun(this, &Render::Controls::OnChangeParameters));
	vbox->PackEnd(b);
}

Gui::TextEntry* Controls::AddParameter(const std::string& label)
{
	vbox->PackEnd(new Gui::Label(label));
	Gui::TextEntry *ent = new Gui::TextEntry();
	vbox->PackEnd(ent);
	return ent;
}

void Controls::GetSizeRequested(float size[2])
{
	size[0] = m_requestedSize.x;
	size[1] = m_requestedSize.y;
}

void Controls::OnChangeParameters()
{
	const float lumb = atof(m_luminanceBias->GetText().c_str());
	const float key  =  atof(m_exposureKey->GetText().c_str());
	RenderParams()->luminanceBias = lumb;
	RenderParams()->key = key;
}

HdrParams* Controls::RenderParams()
{
	static HdrParams *params = new HdrParams;
	return params;
}

} //namespace Render
