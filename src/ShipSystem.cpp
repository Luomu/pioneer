// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "ShipSystem.h"

ShipSystem::ShipSystem(Type t, const std::string &n)
: m_active(true)
, m_health(1.f)
, m_heat(0.f)
, m_powerLevel(0.f)
, m_powerUse(0.f)
, m_name(n)
, m_type(t)
{

}

ShipSystem::~ShipSystem()
{

}

ShipSystem::Status ShipSystem::GetStatus() const
{
	if (m_health < 0.25) return RED;
	else if (m_health < 0.80) return YELLOW;
	else return GREEN;
}

float ShipSystem::RequestPower(float timestep) const
{
	if (m_active) return 0.f;

	return m_powerUse * timestep;
}

void ShipSystem::OnDamage(float damage)
{

}

void ShipSystem::Update(float timestep)
{

}