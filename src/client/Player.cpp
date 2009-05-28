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

namespace backlot
{
	Player::Player()
	{
		visible = false;
		position = Vector2F(0, 0);
		rotation = 0;

		players.push_back(this);
	}
	Player::~Player()
	{
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
		return true;
	}

	void Player::setID(int id)
	{
		this->id = id;
	}
	int Player::getID()
	{
		return id;
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

	void Player::setVisible(bool visible)
	{
		this->visible = visible;
	}
	bool Player::isVisible()
	{
		return visible;
	}

	void Player::render()
	{
	}
	void Player::renderAll()
	{
		for (unsigned int i = 0; i < players.size(); i++)
		{
			players[i]->render();
		}
	}

	std::vector<Player*> Player::players;
}
