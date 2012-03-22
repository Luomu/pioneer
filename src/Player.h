#ifndef _PLAYER_H
#define _PLAYER_H

#include "libs.h"
#include "Ship.h"
#include "ShipController.h"
#include "PlayerCharacter.h" //XXX to reduce the number of include changes

namespace Graphics { class Renderer; }

class Player: public Ship {
public:
	OBJDEF(Player, Ship, PLAYER);
	Player(ShipType::Type shipType);
	Player() { }; //default constructor used before Load

	//XXX temporary things to avoid causing too many changes right now
	Body *GetCombatTarget() const;
	Body *GetNavTarget() const;
	Body *GetSetSpeedTarget() const;
	void SetCombatTarget(Body* const target, bool setSpeedTo = false);
	void SetNavTarget(Body* const target, bool setSpeedTo = false);

	Pioneer::Player *GetPlayer(); //XXX to avoid some lua work
};

#endif /* _PLAYER_H */
