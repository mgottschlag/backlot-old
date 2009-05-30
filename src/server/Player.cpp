/*
Copyright (C) 2009  Mathias Gottschlag

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

namespace backlot
{
	Player::Player() : ReferenceCounted()
	{
		id = ++lastid;
		rotation = 0;
		keys = 0;
		position = Vector2F(0.5, 0.5);
		lastweaponid = 0;
	}
	Player::~Player()
	{
	}

	bool Player::load()
	{
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

	int Player::addWeapon(WeaponPointer weapon)
	{
		WeaponState state;
		state.weapon = weapon;
		state.currentmagazine = weapon->getMagazineSize();
		state.reserve = weapon->getMagazineSize() * weapon->getMagazineCount();
		int id = ++lastweaponid;
		weapons.insert(std::pair<int, WeaponState>(id, state));
		return id;
	}

	void Player::think()
	{
		if (keys & EKM_Move)
		{
			// Change position
			Vector2F newpos = position;
			if (keys & EKM_Up)
			{
				newpos += Vector2F(0, -0.02);
			}
			if (keys & EKM_Down)
			{
				newpos += Vector2F(0, 0.02);
			}
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
	}

	int Player::lastid = 0;
}
