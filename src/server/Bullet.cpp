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

#include "Bullet.hpp"
#include "Server.hpp"

#include <iostream>

namespace backlot
{
	Bullet::Bullet(WeaponPointer weapon, Player *player) : ReferenceCounted(),
		weapon(weapon), player(player)
	{
		bullets.push_back(this);
	}
	Bullet::~Bullet()
	{
	}

	void Bullet::setPosition(Vector2F position)
	{
		this->position = position;
	}
	Vector2F Bullet::getPosition()
	{
		return position;
	}

	void Bullet::setSpeed(Vector2F speed)
	{
		this->speed = speed;
	}
	Vector2F Bullet::getSpeed()
	{
		return speed;
	}

	bool Bullet::update()
	{
		Vector2F newpos = position + speed * 0.02;
		Vector2F hit;
		// Check player hits
		std::vector<Player*> &players = Player::getPlayers();
		for (unsigned int i = 0; i < players.size(); i++)
		{
			// Don't shoot yourself
			if (players[i] == player)
				continue;
			// Get hit point
			Vector2F pos = players[i]->getPosition();
			RectangleF playerrect(pos.x - 0.3, pos.y - 0.3, 0.6, 0.6);
			Vector2F hit2;
			if (playerrect.getIntersections(Line(position, newpos), hit, hit2))
			{
				std::cout << "Player hit." << std::endl;
				return false;
			}
		}
		// Check terrain hits
		if (Server::get().getMap()->isAccessible(position, newpos, &hit))
		{
			position = newpos;
			return true;
		}
		else
		{
			// We've hit a wall
			// TODO
			return false;
		}
	}
	void Bullet::updateAll()
	{
		for (unsigned int i = 0; i < bullets.size(); i++)
		{
			if (!bullets[i]->update())
			{
				bullets.erase(bullets.begin() + i);
				i--;
			}
		}
	}

	std::vector<SharedPointer<Bullet> > Bullet::bullets;
}
