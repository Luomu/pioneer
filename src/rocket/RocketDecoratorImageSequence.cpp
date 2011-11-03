#include "RocketDecoratorImageSequence.h"

RocketDecoratorInstancerImageSequence::RocketDecoratorInstancerImageSequence()
{
	//RegisterProperty("src", "").AddParser("...
	//cell-width
	//cell-height
	//cell-index
	//could be also: num rows, num cols, then calculate from image...
}

RocketDecoratorInstancerImageSequence::~RocketDecoratorInstancerImageSequence()
{
}

Rocket::Core::Decorator* RocketDecoratorInstancerImageSequence::InstanceDecorator(
	const Rocket::Core::String &name, const Rocket::Core::PropertyDictionary &properties)
{
	//get properties

	RocketDecoratorImageSequence *decorator = new RocketDecoratorImageSequence();
	//decorator->Initialize(parsed_properties)
	decorator->RemoveReference();
	ReleaseDecorator(decorator);
	return 0;
}

void RocketDecoratorInstancerImageSequence::ReleaseDecorator(Rocket::Core::Decorator *decorator)
{
	delete decorator;
}

void RocketDecoratorInstancerImageSequence::Release()
{
	delete this;
}