#include "Player.h"
#include "Frame.h"
#include "Game.h"
#include "HyperspaceCloud.h"
#include "KeyBindings.h"
#include "Lang.h"
#include "Pi.h"
#include "SectorView.h"
#include "Serializer.h"
#include "ShipCpanel.h"
#include "Sound.h"
#include "SpaceStation.h"
#include "SpaceStationView.h"
#include "WorldView.h"

Player::Player(ShipType::Type shipType): Ship(shipType)
{
}