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
#include "Client.hpp"
#include "NetworkData.hpp"

#include <GL/gl.h>
#include <iostream>

namespace backlot
{
	Player::Player() : ReferenceCounted()
	{
		visible = false;
		position = Vector2F(0, 0);
		rotation = 0;
		currentweapon = -1;

		players.push_back(this);
	}
	Player::~Player()
	{
		if (local == this)
			local = 0;
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
		texture = new Texture();
		texture->load("sprites/player.png");
		feettexture = new Texture();
		feettexture->load("sprites/feet.png");
		feetanimation = new Animation();
		feetanimation->setFrameNumbers(4, 2);
		feetanimation->setPeriod(2000);
		feetanimation->setLoop(-1);
		feetsound = new Sound();
		feetsound->load("sounds/footsteps.wav");
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

	void Player::setLocal(bool local)
	{
		if (local)
			Player::local = this;
		else if (Player::local == this)
			Player::local = 0;
	}
	bool Player::isLocal()
	{
		return this == local;
	}
	SharedPointer<Player> Player::getLocalPlayer()
	{
		return local;
	}

	void Player::sendKeys(uint8_t keys)
	{
		// Send keyboard info to the server
		BufferPointer buffer = new Buffer();
		buffer->write8(EPT_Keys);
		buffer->write32(id);
		buffer->write8(keys);
		Client::get().send(buffer);
		// TODO: Client side prediction
	}
	void Player::receiveKeys(uint8_t keys)
	{
		this->keys = keys;

		if ((keys & EKM_Move) && feetsound->isPlaying() == false)
			feetsound->play(-1);
		else if ((keys & EKM_Move) == 0 && feetsound->isPlaying() == true)
			feetsound->stop();			
	}
	void Player::sendRotation(float rotation)
	{
		// Send keyboard info to the server
		BufferPointer buffer = new Buffer();
		buffer->write8(EPT_Rotation);
		buffer->write32(id);
		buffer->writeFloat(rotation);
		Client::get().send(buffer);
		// TODO: Client side prediction
	}

	void Player::addWeapon(int id, WeaponPointer weapon)
	{
		WeaponState state;
		state.weapon = weapon;
		state.currentmagazine = weapon->getMagazineSize();
		state.reserve = weapon->getMagazineSize() * weapon->getMagazineCount();
		weapons.insert(std::pair<int, WeaponState>(id, state));
		if (currentweapon == -1)
			currentweapon = id;
	}

	void Player::render()
	{
		if (!visible)
			return;

		glPushMatrix();	
		// If the player moved and the feed animation is not played
		glTranslatef(position.x, position.y, 10.0);
		glScalef(0.5, 0.5, 1.0);

		if (feetanimation->isPlaying() == false && keys & EKM_Move)
			feetanimation->start();
		else if (feetanimation->isPlaying() == true && (keys & EKM_Move) == 0)
			feetanimation->stop();

		if (keys & EKM_Move)
		{
			if (keys & EKM_Right)
				glRotatef(270, 0, 0, 1);
			if (keys & EKM_Down || keys & EKM_Up)
				glRotatef(0, 0, 0, 1);
			if (keys & EKM_Left)
				glRotatef(90, 0, 0, 1);
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		feettexture->bind();

		if (feetanimation->isPlaying() == false)
		{
			glRotatef(rotation * 180 / 3.1415, 0.0, 0.0, 1.0);
			feetanimation->draw(0);
		}
		else
			feetanimation->draw();

		// Setup position
		glLoadIdentity();
		glTranslatef(position.x, position.y, 10.0);
		glRotatef(rotation * 180 / 3.1415, 0.0, 0.0, 1.0);
		// Enable Alpha channel blending
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Draw player
		texture->bind();
		glBegin(GL_QUADS);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(-0.5, -0.5, 0.0);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(0.5, -0.5, 0.0);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(0.5, 0.5, 0.0);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(-0.5, 0.5, 0.0);
		glEnd();
		// Draw weapon
		if (currentweapon != -1)
		{
			weapons[currentweapon].weapon->getPlayerTexture()->bind();
			glBegin(GL_QUADS);
				glTexCoord2f(1.0, 1.0);
				glVertex3f(-0.5, -0.5, 0.0);
				glTexCoord2f(0.0, 1.0);
				glVertex3f(0.5, -0.5, 0.0);
				glTexCoord2f(0.0, 0.0);
				glVertex3f(0.5, 0.5, 0.0);
				glTexCoord2f(1.0, 0.0);
				glVertex3f(-0.5, 0.5, 0.0);
			glEnd();
		}
		// Clean up
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
	}
	void Player::renderAll()
	{
		for (unsigned int i = 0; i < players.size(); i++)
		{
			players[i]->render();
		}
	}

	std::vector<Player*> Player::players;

	Player *Player::local = 0;
}
