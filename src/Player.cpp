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

//temporary targeting stuff
Body *Player::GetCombatTarget() const
{
	return static_cast<PlayerShipController*>(m_controller)->GetCombatTarget();
}

Body *Player::GetNavTarget() const
{
	return static_cast<PlayerShipController*>(m_controller)->GetNavTarget();
}

Body *Player::GetSetSpeedTarget() const
{
	return static_cast<PlayerShipController*>(m_controller)->GetSetSpeedTarget();
}

void Player::SetCombatTarget(Body* const target, bool setSpeedTo)
{
	static_cast<PlayerShipController*>(m_controller)->SetCombatTarget(target, setSpeedTo);
	Pi::onPlayerChangeTarget.emit();
}

void Player::SetNavTarget(Body* const target, bool setSpeedTo)
{
	static_cast<PlayerShipController*>(m_controller)->SetNavTarget(target, setSpeedTo);
	Pi::onPlayerChangeTarget.emit();
}
//temporary targeting stuff ends

Pioneer::Player *Player::GetPlayer()
{
	return Pi::game->GetPlayer();
}