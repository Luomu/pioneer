// Copyright � 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "Thrusters.h"

Thrusters::Thrusters()
: ShipSystem(THRUSTER, "Thrusters")
{
	m_maxEnergy = 500.f;
	m_powerUseRate = 500.f;
}
