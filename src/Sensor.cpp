#include "Object.h"
#include "Space.h"
#include "Pi.h"
#include "Sensor.h"

Contact::Contact(Body *b) :
	m_body(b)
{
}

Contact::~Contact()
{
}

Sensor::Sensor()
{
}

Sensor::~Sensor()
{
}

void Sensor::Update(const float timeStep)
{
	CollectContacts();
}

void Sensor::CollectContacts()
{
	m_contacts.clear();

	for (Space::bodiesIter_t i = Space::bodies.begin(); i != Space::bodies.end(); ++i) {
		//if ((*i) == Pi::player) continue;

		//float dist = float((*i)->GetPositionRelTo(Pi::player).Length());
		//if (dist > SCANNER_RANGE_MAX) continue;

		Contact c((*i));

		switch ((*i)->GetType()) {
			case Object::SHIP:
				break;
			case Object::STAR:
			case Object::PLANET:
				break;
			default:
				continue;
		}

		m_contacts.push_back(c);
	}
}
