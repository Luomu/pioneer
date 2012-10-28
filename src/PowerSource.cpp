// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "PowerSource.h"

PowerSource::PowerSource()
: ShipSystem(POWERSOURCE, "Reactor")
, m_output(1000.f)
{
}

void PowerSource::Update(float time)
{
	float powerAvailable = m_active ? m_output * m_powerLevel * time : 0.f;

	std::vector<ShipSystem*>::iterator it = m_consumers.begin();
	for (; it != m_consumers.end(); ++it) {
		ShipSystem *system = (*it);

		const float request = std::min(powerAvailable, system->RequestPower(time));

		if (request > 0.f) {
			//energize the system
			system->DeliverEnergy(request, time);
			powerAvailable -= request;
		}
	}

	//using these to show used % on worldview
	m_maxEnergy = m_output;
	m_energy = powerAvailable/std::max(0.001f, time);

	//add heat depending on power use
}

void PowerSource::AddConsumer(ShipSystem *c)
{
	m_consumers.push_back(c);
}

void PowerSource::RemoveConsumer(ShipSystem *c)
{
	std::vector<ShipSystem*>::iterator it = m_consumers.begin();
	for (; it != m_consumers.end(); ++it) {
		if ((*it) == c) {
			m_consumers.erase(it);
			return;
		}
	}
}

