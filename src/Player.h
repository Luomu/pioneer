#pragma once
/*
 * The player character. Controls one ship at a time.
 */
#include "MarketAgent.h"
#include "RefList.h"
#include "Serializer.h"

class Ship;

struct Mission : RefItem<Mission> {
	enum MissionState { // <enum scope='Mission' name=MissionStatus>
		ACTIVE,
		COMPLETED,
		FAILED,
	};

	std::string  type;
	std::string  client;
	SystemPath   location;
	double       due;
	Sint64       reward;
	MissionState status;
};

namespace Pioneer {

class Player : public MarketAgent, public DeleteEmitter {
public:
	Player();
	~Player();
	void Save(Serializer::Writer &wr);
	void Load(Serializer::Reader &rd);
	RefList<Mission> missions;
	//get the currently piloted ship
	Ship *GetShip() const;
	//give player control over a ship.
	//The previous one is set to autopilot (and set speed 0?).
	//does not change viewpoint, do that separately
	//XXX actually this changes viewpoint too for now
	void SetShip(Ship *s);

	int GetStock(Equip::Type t) const { assert(0); return 0; }
	bool CanBuy(Equip::Type t, bool verbose) const;
	bool CanSell(Equip::Type t, bool verbose) const;
	bool DoesSell(Equip::Type t) const { return true; }
	Sint64 GetPrice(Equip::Type t) const;

	//XXX temporary things to avoid causing too many changes right now
	Body *GetCombatTarget() const;
	Body *GetNavTarget() const;
	Body *GetSetSpeedTarget() const;
	void SetCombatTarget(Body* const target, bool setSpeedTo = false);
	void SetNavTarget(Body* const target, bool setSpeedTo = false);

protected:
	void Bought(Equip::Type t);
	void Sold(Equip::Type t);

private:
	Ship *m_ship; //current ship
};

}
