/*
Copyright (C) 2009  Mathias Gottschlag, Simon Kerler

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Player.hpp"
#include "NetworkData.hpp"
#include "Server.hpp"
#include "Engine.hpp"
#include "Bullet.hpp"

#include <cmath>
#include <iostream>

namespace backlot
{
	Player::Player() : ReferenceCounted()
	{
		id = ++lastid;
		rotation = 0;
		keys = 0;
		position = Vector2F(0.5, 0.5);
		lastweaponid = 0;
		currentweapon = -1;
		team = 0;
		// Add player to player list
		players.push_back(this);
	}
	Player::~Player()
	{
		// Remove player from list
		for (unsigned int i = 0; i < players.size(); i++)
		{
			if (players[i] == this)
			{
				players.erase(players.begin() + i);
				break;
			}
		}
	}

	bool Player::load()
	{
		hitpoints = 100;
		return true;
	}

	int Player::getID()
	{
		return id;
	}

	void Player::setOwner(Client *owner)
	{
		this->owner = owner;
	}
	Client *Player::getOwner()
	{
		return owner;
	}

	void Player::setPosition(Vector2F position)
	{
		this->position = position;
	}
	Vector2F Player::getPosition()
	{
		return position;
	}

	void Player::setRotation(float rotation)
	{
		this->rotation = rotation;
	}
	float Player::getRotation()
	{
		return rotation;
	}

	void Player::setKeys(uint8_t keys)
	{
		this->keys = keys;
	}
	uint8_t Player::getKeys()
	{
		return keys;
	}

	int Player::getHitpoints()
	{
		return hitpoints;
	}
	void Player::setHitpoints(int newhitpoints)
	{
		hitpoints = newhitpoints;
	}

	void Player::setTeam(int team)
	{
		this->team = team;
	}
	int Player::getTeam()
	{
		return team;
	}

	int Player::addWeapon(WeaponPointer weapon)
	{
		WeaponState state;
		state.weapon = weapon;
		state.currentmagazine = weapon->getMagazineSize();
		state.reserve = weapon->getMagazineSize() * weapon->getMagazineCount();
		state.lastshot = Engine::getTime();
		int id = ++lastweaponid;
		weapons.insert(std::pair<int, WeaponState>(id, state));
		if (currentweapon == -1)
			currentweapon = id;
		return id;
	}
	std::map<int, WeaponState> &Player::getWeapons()
	{
		return weapons;
	}

	std::vector<Player*> &Player::getPlayers()
	{
		return players;
	}

	void Player::think()
	{
		// Movement
		if (keys & EKM_Move)
		{
			// Change vertical position
			Vector2F newpos = position;
			if (keys & EKM_Up)
			{
				newpos += Vector2F(0, -0.02);
			}
			if (keys & EKM_Down)
			{
				newpos += Vector2F(0, 0.02);
			}
			// Check whether position is valid
			if (Server::get().getMap()->isAccessible(RectangleF(newpos.x - 0.35,
				newpos.y - 0.35, 0.7, 0.7)))
			{
				position = newpos;
			}
			else
			{
				newpos = position;
			}
			// Change horizontal position
			if (keys & EKM_Right)
			{
				newpos += Vector2F(0.02, 0);
			}
			if (keys & EKM_Left)
			{
				newpos += Vector2F(-0.02, 0);
			}
			// Check whether position is valid
			if (Server::get().getMap()->isAccessible(RectangleF(newpos.x - 0.35,
				newpos.y - 0.35, 0.7, 0.7)))
			{
				position = newpos;
			}
		}
		// Handle shoot events
		if (keys & EKM_Shoot)
		{
			if (currentweapon != -1)
			{
				WeaponState &state = weapons[currentweapon];
				// Check weapon speed
				uint64_t currenttime = Engine::get().getTime();
				uint64_t shotperiod = 60 * 1000000 / state.weapon->getShotsPerMinute();
				// If the weapon has reloaded.
				if (currenttime - state.lastshot >= shotperiod)
				{
					// If the magazine is not empty.
					if (state.currentmagazine > 0)
					{
						state.lastshot = currenttime;
						state.currentmagazine--;
						// Shoot
						BulletPointer bullet = new Bullet(state.weapon, this);
						bullet->setPosition(position);
						Vector2F speed(sin(rotation), -cos(rotation));
						speed *= state.weapon->getBulletSpeed();
						bullet->setSpeed(speed);
						// Send bullet message to all clients
						BufferPointer buffer = new Buffer();
						buffer->write8(EPT_Projectile);
						buffer->writeFloat(position.x);
						buffer->writeFloat(position.y);
						buffer->writeFloat(speed.x);
						buffer->writeFloat(speed.y);
						buffer->write32(state.weapon->getID());
						Server::get().sendToAll(buffer);
						// Send new weaponstates to the player who shot.
						buffer->setPosition(0);
						buffer->write8(EPT_UpdateWeapon);
						buffer->write8(state.currentmagazine);
						buffer->write8(state.reserve);
						owner->send(buffer);
					}
					// If the player has still bullets left, reload the weapon.
					else if (state.reserve > 0)
					{
						int magazinesize = state.weapon->getMagazineSize();

						if (state.reserve < magazinesize)
						{
							state.currentmagazine = state.reserve;
							state.reserve = 0;
						}
						else
						{
							state.currentmagazine = magazinesize;
							state.reserve -= magazinesize;
						}
						// Send new weaponstates to the player who shot.
						BufferPointer buffer = new Buffer();
						buffer->write8(EPT_UpdateWeapon);
						buffer->write8(state.currentmagazine);
						buffer->write8(state.reserve);
						owner->send(buffer);
					} // else if (state.reserve > 0)
				} // if (currenttime - state.lastshot >= shotperiod)
			} // if (currentweapon != -1)
		}
	} // Player::think()

	std::vector<Player*> Player::players;

	int Player::lastid = 0;
}
