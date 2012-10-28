// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _SHIPSYSTEM_H_
#define _SHIPSYSTEM_H_

#include "libs.h"

class Ship;

/*
 * Ship subsystem
 * Systems:
 *  - perform their function
 *  - use or produce power
 *  - produce or reduce heat
 *  - can be damaged
 *  - can be repaired
 *  - may be deactivated
 */
class ShipSystem {
public:
	enum Type {
		HYPERDRIVE,
		POWERSOURCE,
		RADIATOR,
		SCOOP,
		SENSOR,
		SHIELD,
		THRUSTER,
		WEAPON,
	};
	enum Status { //overall status
		GREEN,
		YELLOW,
		RED
	};
	ShipSystem(Type, const std::string &name);
	virtual ~ShipSystem();

	bool GetActive() const { return m_active; }
	const std::string &GetName() const { return m_name; }
	float GetCharge() const { return m_energy / m_maxEnergy; }
	float GetHealth() const { return m_health; }
	float GetHeat() const { return m_heat; }
	float GetPowerLevel() const { return m_powerLevel; }
	Status GetStatus() const;
	Type GetType() const { return m_type; }

	void SetActive(bool b) { m_active = b; } //switches state instantly, but might want to add a delay
	void SetPowerLevel(float f) { m_powerLevel = f; }
	void SetShip(Ship *s) { m_ship = s; }

	virtual float RequestPower(float time) const;
	virtual void AddHeat(float heat);
	virtual void AddPower(float power);
	virtual void OnDamage(float damage);
	virtual void Repair(float time);
	virtual void Update(float time);
	void SetRepair(bool);

protected:
	bool m_active;
	bool m_repair;
	float m_energy; // joules or some other fancy unit
	float m_health;
	float m_heat;
	float m_maxEnergy;
	float m_powerLevel; // 0-1. A switched on system should be 1 (penalties/bonuses might affect this)
	float m_powerUseRate;
	Ship *m_ship;
	std::string m_name;
	Type m_type;
};

#endif
