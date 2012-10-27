// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _SHIPSYSTEM_H_
#define _SHIPSYSTEM_H_

#include "libs.h"
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

	const std::string &GetName() const { return m_name; }
	float GetHealth() const { return m_health; }
	float GetPowerLevel() const { return m_powerLevel; }
	Status GetStatus() const;
	Type GetType() const { return m_type; }

	virtual float RequestPower(float time) const;
	virtual void OnDamage(float damage);
	virtual void Update(float time);

protected:
	bool m_active;
	float m_health;
	float m_heat;
	float m_powerLevel;
	float m_powerUse;
	std::string m_name;
	Type m_type;
};

#endif
