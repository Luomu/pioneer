#ifndef _ROCKETDECORATORIMAGESEQUENCE_H
#define _ROCKETDECORATORIMAGESEQUENCE_H

/*
 * Render a portion of an image (cell), somewhat like the default image decorator
 * except make it possible to specify cell number instead of just begin/end-s/t.
 * The tile is stretched to fit the element, there is no repeat or flip control.
 * Since Rocket does not handle decorator updates, this will need an element or something
 * to monitor the changes.
 */

#include "Rocket/Core/Decorator.h"

class RocketDecoratorImageSequence : public Rocket::Core::Decorator {
public:
	RocketDecoratorImageSequence();
	virtual ~RocketDecoratorImageSequence();
	//DecoratorDataHandle
	//ReleaseElementData
	//RenderElement
};

class RocketDecoratorInstancerImageSequence : public Rocket::Core::DecoratorInstancer
{
public:
	RocketDecoratorInstancerImageSequence();
	virtual ~RocketDecoratorInstancerImageSequence();

	Rocket::Core::Decorator* InstanceDecorator(const Rocket::Core::String &name,
		const Rocket::Core::PropertyDictionary &properties);
	void ReleaseDecorator(Rocket::Core::Decorator *decorator);
	void Release();
};

#endif
