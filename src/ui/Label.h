// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _UI_LABEL_H
#define _UI_LABEL_H

#include "Widget.h"
#include "SmartPtr.h"

// single line of text

namespace UI {

class Label: public Widget {
public:
	virtual Point PreferredSize();
	virtual void Layout();
	virtual void Draw();

	Label *SetText(const std::string &text);

protected:
	friend class Context;
	Label(Context *context, const std::string &text) : Widget(context), m_text(text) {}

private:
	std::string m_text;
	Point m_preferredSize;
};

}

#endif