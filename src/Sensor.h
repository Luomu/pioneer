// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "ShipSystem.h"

/*
 * Provides targeting functionality
 * Scanner widget reads the contact list from this
 */
class Sensor : public ShipSystem {
public:
	Sensor();
	virtual void Update(float time);
};

#endif
