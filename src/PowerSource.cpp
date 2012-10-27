// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "PowerSource.h"

PowerSource::PowerSource()
: ShipSystem(POWERSOURCE, "Reactor")
{
}

void PowerSource::Update(float time)
{
	std::vector<ShipSystem*>::iterator it = m_consumers.begin();
	for (; it != m_consumers.end(); ++it) {
		float request = (*it)->RequestPower(time);

		//energize the system, if power available
	}
}

void PowerSource::AddConsumer(ShipSystem *c)
{
	m_consumers.push_back(c);
}