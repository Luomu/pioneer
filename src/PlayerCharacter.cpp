#include "PlayerCharacter.h"
#include "Pi.h"
#include "Ship.h"
#include "Lang.h"

namespace Pioneer {

Player::Player() : MarketAgent() { }

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

Ship *Player::GetCurrentShip() const
{
	return Pi::player;
}

void Player::Bought(Equip::Type t)
{
	GetCurrentShip()->m_equipment.Add(t);
	GetCurrentShip()->UpdateMass(); //XXX shouldn't equip add do this
}

void Player::Sold(Equip::Type t)
{
	GetCurrentShip()->m_equipment.Remove(t, 1);
	GetCurrentShip()->UpdateMass();
}

bool Player::CanBuy(Equip::Type t, bool verbose) const
{
	Equip::Slot slot = Equip::types[int(t)].slot;
	const bool freespace = (GetCurrentShip()->m_equipment.FreeSpace(slot)!=0);
	const bool freecapacity = (GetCurrentShip()->m_stats.free_capacity >= Equip::types[int(t)].mass);
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
	const bool cansell = (GetCurrentShip()->m_equipment.Count(slot, t) > 0);
	if (verbose) {
		if (!cansell) {
			Pi::Message(stringf(Lang::YOU_DO_NOT_HAVE_ANY_X, formatarg("item", Equip::types[int(t)].name)));
		}
	}
	return cansell;
}

Sint64 Player::GetPrice(Equip::Type t) const
{
	if (GetCurrentShip()->GetDockedWith()) {
		return GetCurrentShip()->GetDockedWith()->GetPrice(t);
	} else {
		assert(0);
		return 0;
	}
}

}
