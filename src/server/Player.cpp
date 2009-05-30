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

namespace backlot
{
	Player::Player() : ReferenceCounted()
	{
		id = ++lastid;
		rotation = 0;
		keys = 0;
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

	void Player::think()
	{
		if (keys & EKM_Move)
		{
			if (keys & EKM_Up)
			{
				position += Vector2F(0, -0.01);
			}
			if (keys & EKM_Down)
			{
				position += Vector2F(0, 0.01);
			}
			if (keys & EKM_Right)
			{
				position += Vector2F(0.01, 0);
			}
			if (keys & EKM_Left)
			{
				position += Vector2F(-0.01, 0);
			}
		}
	}

	int Player::lastid = 0;
}
