// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "FuelScoop.h"
#include "Frame.h"
#include "Lang.h"
#include "Pi.h"
#include "Planet.h"
#include "Ship.h"

FuelScoop::FuelScoop(Ship *s)
: ShipSystem(SCOOP, "Fuel scoop")
{
	SetShip(s);
}

void FuelScoop::Update(float time)
{
	//note: only the player can scoop
	if (
		!m_ship->IsType(Object::PLAYER) ||
		m_ship->GetFlightState() != Ship::FLYING ||
		m_ship->GetEquipment().Get(Equip::SLOT_FUELSCOOP) == Equip::NONE)
	return;

	SetPowerLevel(0.2f); //standby power

	Body *astro = m_ship->GetFrame()->m_astroBody;
	if (astro && astro->IsType(Object::PLANET)) {
		Planet *p = static_cast<Planet*>(astro);
		if (p->GetSystemBody()->IsScoopable()) {
			double dist = m_ship->GetPosition().Length();
			double pressure, density;
			p->GetAtmosphericState(dist, &pressure, &density);

			const double speed = m_ship->GetVelocity().Length();
			vector3d vdir = m_ship->GetVelocity().Normalized();
			matrix4x4d rot;
			m_ship->GetRotMatrix(rot);
			const vector3d pdir = -vector3d(rot[8], rot[9], rot[10]).Normalized();
			const double dot = vdir.Dot(pdir);
			//The ship must be flying approximately horizontally and fast enough
			if ((m_ship->GetStats().free_capacity) && (dot > 0.95) && (speed > 2000.0) && (density > 1.0)) {
				SetPowerLevel(1.f);
				const double rate = speed*density*0.00001f;
				if (Pi::rng.Double() < rate) {
					m_ship->GetEquipment().Add(Equip::HYDROGEN);
					m_ship->UpdateEquipStats();
					Pi::Message(stringf(Lang::FUEL_SCOOP_ACTIVE_N_TONNES_H_COLLECTED,
						formatarg("quantity", m_ship->GetEquipment().Count(Equip::SLOT_CARGO, Equip::HYDROGEN))));
				}
			}
		}
	}
}
