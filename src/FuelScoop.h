// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef _FUELSCOOP_H_
#define _FUELSCOOP_H_

#include "ShipSystem.h"

/*
 * Slurps hydrogen from gas giant atmospheres
 */
class FuelScoop : public ShipSystem {
public:
	FuelScoop(Ship *s);

	virtual void Update(float time);

};

#endif
