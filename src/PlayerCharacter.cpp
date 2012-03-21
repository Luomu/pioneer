#include "PlayerCharacter.h"
#include "Pi.h"
#include "Ship.h"
#include "Lang.h"

namespace Pioneer {

Player::Player() : MarketAgent(), m_ship(0) { }

Player::~Player() { }

void Player::Save(Serializer::Writer &wr)
{
	MarketAgent::Save(wr);
	//missions are serialized by lua scripts
}

void Player::Load(Serializer::Reader &rd)
{
	MarketAgent::Load(rd);
}

Ship *Player::GetShip() const
{
	return m_ship;
}

void Player::SetShip(Ship *ship)
{
	Ship *currentShip = GetShip();
	//XXX could be better to just Deactivate the current PlayerShipController.
	//currentShip->SetController(new ShipController());
	if (currentShip) {
		currentShip->GetController()->m_active = false;
		currentShip->isPlayerShip = false;
	}
	ship->SetController(new PlayerShipController());
	ship->isPlayerShip = true;
	m_ship = ship;
	if (Pi::worldView) Pi::worldView->SetCameraBody(ship);
}

void Player::Bought(Equip::Type t)
{
	GetShip()->m_equipment.Add(t);
	GetShip()->UpdateMass(); //XXX shouldn't equip add do this
}

void Player::Sold(Equip::Type t)
{
	GetShip()->m_equipment.Remove(t, 1);
	GetShip()->UpdateMass();
}

bool Player::CanBuy(Equip::Type t, bool verbose) const
{
	Equip::Slot slot = Equip::types[int(t)].slot;
	const bool freespace = (GetShip()->m_equipment.FreeSpace(slot)!=0);
	const bool freecapacity = (GetShip()->m_stats.free_capacity >= Equip::types[int(t)].mass);
	if (verbose) {
		if (!freespace) {
			Pi::Message(Lang::NO_FREE_SPACE_FOR_ITEM);
		}
		else if (!freecapacity) {
			Pi::Message(Lang::SHIP_IS_FULLY_LADEN);
		}
	}
	return (freespace && freecapacity);
}

bool Player::CanSell(Equip::Type t, bool verbose) const
{
	Equip::Slot slot = Equip::types[int(t)].slot;
	const bool cansell = (GetShip()->m_equipment.Count(slot, t) > 0);
	if (verbose) {
		if (!cansell) {
			Pi::Message(stringf(Lang::YOU_DO_NOT_HAVE_ANY_X, formatarg("item", Equip::types[int(t)].name)));
		}
	}
	return cansell;
}

Sint64 Player::GetPrice(Equip::Type t) const
{
	if (GetShip()->GetDockedWith()) {
		return GetShip()->GetDockedWith()->GetPrice(t);
	} else {
		assert(0);
		return 0;
	}
}

}
