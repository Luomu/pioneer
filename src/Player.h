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
};

#endif /* _PLAYER_H */
