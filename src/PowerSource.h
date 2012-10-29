// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _POWERSOURCE_H_
#define _POWERSOURCE_H_

#include "ShipSystem.h"

/*
 * Reactor of some sort. Produces power, does not use any.
 * Produces plenty of heat.
 */
class PowerSource : public ShipSystem {
public:
	PowerSource();

	virtual void Update(float time);

	void AddConsumer(ShipSystem*);
	void RemoveConsumer(ShipSystem*);

private:
	std::vector<ShipSystem*> m_consumers;
	float m_output;
};

#endif
