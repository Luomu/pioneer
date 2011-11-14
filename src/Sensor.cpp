#include "Object.h"
#include "Space.h"
#include "Player.h"
#include "Pi.h"
#include "Sensor.h"

Contact::Contact(Body *b) :
	m_body(b)
{
}

bool Contact::IsNavTarget() const
{
	return m_body == Pi::player->GetNavTarget() ? true : false;
}

bool Contact::IsCombatTarget() const
{
	return m_body == Pi::player->GetCombatTarget() ? true : false;
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
	// Gather knowledge of following bodies:
	// Stellar & planetary bodies: always
	// Space stations: always
	// Cities: when within range
	// Ships & missiles: when within range
	// Script-pinned targets: always
	//
	// It is up to the visual widgets what and how to present the contacts
	m_contacts.clear();

	for (Space::bodiesIter_t i = Space::bodies.begin(); i != Space::bodies.end(); ++i) {
		//if ((*i) == Pi::player) continue;

		//float dist = float((*i)->GetPositionRelTo(Pi::player).Length());
		//if (dist > SCANNER_RANGE_MAX) continue;

		Contact c((*i));

		switch ((*i)->GetType()) {
			case Object::SHIP:
				break;
			case Object::SPACESTATION:
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
