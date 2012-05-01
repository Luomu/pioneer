#ifndef _UI_CHECKBOX_H
#define _UI_CHECKBOX_H

#include "Widget.h"

namespace UI {

class Checkbox: public Widget {
public:
	virtual vector2f PreferredSize();
	virtual void Layout();
	virtual void Draw();

protected:
	friend class Context;
	Checkbox(Context *context): Widget(context), m_checked(false) {}

	void HandleClick();

private:
	bool m_checked;
};

}

#endif