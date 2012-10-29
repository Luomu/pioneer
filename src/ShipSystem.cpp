// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "ShipSystem.h"

ShipSystem::ShipSystem(Type t, const std::string &n)
: m_active(true)
, m_repair(true)
, m_energy(0.f)
, m_health(1.f)
, m_heat(0.f)
, m_maxEnergy(100.f)
, m_powerLevel(1.f)
, m_powerUseRate(100.f)
, m_ship(0)
, m_name(n)
, m_type(t)
{

}

ShipSystem::~ShipSystem()
{

}

ShipSystem::Status ShipSystem::GetStatus() const
{
	//I just picked some values
	if (m_health < 0.25) return RED;
	else if (m_health < 0.80) return YELLOW;
	else return GREEN;
}

float ShipSystem::RequestPower(float timestep) const
{
	//if (!m_active || m_energy >= m_maxEnergy) return 0.f;
	if (!m_active) return 0.f;

	return m_powerLevel * m_powerUseRate * timestep;
}

void ShipSystem::AddHeat(float heat)
{
	m_heat += heat;
}

void ShipSystem::DeliverEnergy(float energy, float time)
{
	//avoid zero division
	time = std::max(time, 0.00001f);

	//basic systems do not accumulate energy
	m_energy = Clamp(energy/time, 0.f, m_maxEnergy);
}

void ShipSystem::OnDamage(float damage)
{
	m_repair = true;
	m_health -= damage;
}

void ShipSystem::Repair(float timestep)
{
	//repairing everything at a constant rate - this will be handled
	//by something more complicated (crew)
	if (m_repair) {
		m_health = std::min(m_health + 0.01f * timestep, 1.f);
		if (m_health >= 1.f)
			m_repair = false;
	}
}

void ShipSystem::Update(float timestep)
{
	//Do system functionality updates here
	//Spend energy, produce heat
	//Low energy, heat or damage may affect operation
	//Repair is done elsewhere
}

void ShipSystem::SetRepair(bool r)
{
	m_repair = true;
}
