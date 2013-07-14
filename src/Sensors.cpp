#include "Sensors.h"
#include "Body.h"
#include "Colors.h"
#include "Game.h"
#include "Pi.h"
#include "Ship.h"
#include "Space.h"
#include "Player.h"
#include "HudTrail.h"

Sensors::RadarContact::RadarContact()
: body(0)
, trail(0)
, distance(0.0)
, iff(IFF_UNKNOWN)
, fresh(true) {
}

Sensors::RadarContact::~RadarContact() {
	body = 0;
	delete trail;
}

Color Sensors::IFFColor(IFF iff)
{
	switch (iff)
	{
		case IFF_NEUTRAL: return Colors::IFF_NEUTRAL;
		case IFF_ALLY:    return Colors::IFF_ALLY;
		case IFF_HOSTILE: return Colors::IFF_HOSTILE;
		case IFF_UNKNOWN:
		default:
			return Colors::IFF_UNKNOWN;
	}
}

bool Sensors::ContactDistanceSort(const RadarContact &a, const RadarContact &b)
{
	return a.distance < b.distance;
}

Sensors::Sensors(Ship *owner)
{
	m_owner = owner;
}

bool Sensors::ChooseTarget(TargetingCriteria crit)
{
	bool found = false;

	m_radarContacts.sort(ContactDistanceSort);

	for (auto it = m_radarContacts.begin(); it != m_radarContacts.end(); ++it) {
		//match object type
		//match iff
		if (it->body->IsType(Object::SHIP)) {
			if (it->iff != IFF_HOSTILE) continue;
			//should move the target to ship after all (from PlayerShipController)
			//targeting inputs stay in PSC
			static_cast<Player*>(m_owner)->SetCombatTarget(it->body);
			bool found = true;
			break;
		}
	}

	return found;
}

Sensors::IFF Sensors::CheckIFF(Body* other)
{
	//complicated relationship check goes here
	if (other->IsType(Object::SHIP)) {
		Ship *s = static_cast<Ship*>(other);
		Uint8 rel = m_owner->GetRelations(other);
		if (rel == 0) return IFF_HOSTILE;
		else if (rel == 100) return IFF_ALLY;
		return IFF_NEUTRAL;
	} else {
		return IFF_UNKNOWN;
	}
}

void Sensors::Update(float time)
{
	if (m_owner != Pi::player) return;

	//Find nearby contacts, same range as scanner. Scanner should use these
	//contacts, worldview labels too.
	Space::BodyNearList nearby;
	Pi::game->GetSpace()->GetBodiesMaybeNear(m_owner, 100000.0f, nearby);
	for (Space::BodyNearIterator i = nearby.begin(); i != nearby.end(); ++i) {
		if ((*i) == m_owner || !(*i)->IsType(Object::SHIP)) continue;
		if ((*i)->IsDead()) continue;

		auto cit = m_radarContacts.begin();
		while (cit != m_radarContacts.end()) {
			if (cit->body == (*i)) break;
			++cit;
		}

		//create new contact or refresh old
		if (cit == m_radarContacts.end()) {
			m_radarContacts.push_back(RadarContact());
			RadarContact &rc = m_radarContacts.back();
			rc.body = (*i);
			rc.iff = CheckIFF(rc.body);
			rc.trail = new HudTrail(rc.body, IFFColor(rc.iff));
		} else {
			cit->fresh = true;
		}
	}

	//update contacts and delete stale ones
	auto it = m_radarContacts.begin();
	while (it != m_radarContacts.end()) {
		if (!it->fresh) {
			m_radarContacts.erase(it++);
		} else {
			it->distance = m_owner->GetPositionRelTo(it->body).Length();
			it->trail->Update(time);
			it->fresh = false;
			++it;
		}
	}
}

void Sensors::UpdateIFF(Body *b)
{
	for (auto it = m_radarContacts.begin(); it != m_radarContacts.end(); ++it)
	{
		if (it->body == b) {
			it->iff = CheckIFF(b);
			it->trail->SetColor(IFFColor(it->iff));
		}
	}
}

