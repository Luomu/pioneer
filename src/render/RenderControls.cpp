#include "RenderControls.h"
#include <sstream>

namespace Render {

Controls::Controls() :
	m_requestedSize(100, 1000)
{
	vbox = new Gui::VBox();
	Add(vbox, 0, 0);

	//defaults
	const HdrParams& p = RenderParams();

	AddParameter("LuminanceBias",       p.luminanceBias);
	AddParameter("KeyValue",            p.key);
	AddParameter("LuminanceSaturation", p.luminanceSaturation);
	AddParameter("WhiteLevel",          p.whiteLevel);

	Gui::LabelButton *b = new Gui::LabelButton(new Gui::Label("Apply"));
	b->onClick.connect(sigc::mem_fun(this, &Render::Controls::OnChangeParameters));
	vbox->PackEnd(b);
}

Gui::TextEntry* Controls::AddParameter(const std::string& label, float val)
{
	vbox->PackEnd(new Gui::Label(label));
	Gui::TextEntry *ent = new Gui::TextEntry();
	vbox->PackEnd(ent);
	m_entries[label] = ent;
	std::ostringstream ss;
	ss << val;
	ent->SetText(ss.str());
	return ent;
}

void Controls::GetSizeRequested(float size[2])
{
	size[0] = m_requestedSize.x;
	size[1] = m_requestedSize.y;
}

float Controls::GetFloat(const std::string &key)
{
	Gui::TextEntry *ent = m_entries[key];
	float val = atof(ent->GetText().c_str());
	return val;
}

void Controls::OnChangeParameters()
{
	HdrParams &p = RenderParams();
	p.luminanceBias       = GetFloat("LuminanceBias");
	p.key                 = GetFloat("KeyValue");
	p.luminanceSaturation = GetFloat("LuminanceSaturation");
	p.whiteLevel          = GetFloat("WhiteLevel");
}

HdrParams& Controls::RenderParams()
{
	static HdrParams *params = new HdrParams;
	return *(params);
}

} //namespace Render
