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

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include "ReferenceCounted.hpp"
#include "Vector2.hpp"
#include "Weapon.hpp"

#include <vector>

namespace backlot
{
	class Client;
	/**
	 * Server-side player information.
	 */
	class Player : public ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 */
			Player();
			/**
			 * Destructor.
			 */
			~Player();

			/**
			 * Initializes the player.
			 */
			bool load();

			/**
			 * Returns the unique identification number of the player. This
			 * number is the same on the server and the client.
			 */
			int getID();

			/**
			 * Sets the owner of the player. Only the owner is allowed to
			 * control a player.
			 */
			void setOwner(Client *owner);
			/**
			 * Returns the owner of the player.
			 */
			Client *getOwner();

			/**
			 * Sets the position of the player.
			 */
			void setPosition(Vector2F position);
			/**
			 * Returns the position of the player.
			 */
			Vector2F getPosition();

			/**
			 * Sets the rotation of the player in radians.
			 */
			void setRotation(float rotation);
			/**
			 * Returns the rotation of the player.
			 */
			float getRotation();

			/**
			 * Sets the keyboard status of the player. This is sent directly
			 * from the client and includes mouse buttons. The bits are set
			 * according to KeyMask.
			 */
			void setKeys(uint8_t keys);
			/**
			 * Returns the keyboard status of the player.
			 */
			uint8_t getKeys();

			/**
			 * Returns the current hitpoints of the player.
			 */
			int getHitpoints();
			/**
			 * Sets the hitpoints of the player.
			 */
			void setHitpoints(int newhitpoints);

			/**
			 * Sets the team of the player.
			 */
			void setTeam(int team);
			/**
			 * Returns the team of the player.
			 */
			int getTeam();

			/**
			 * Adds a weapon to the player. The weapon is not automatically
			 * sent to all clients.
			 */
			int addWeapon(WeaponPointer weapon);
			/**
			 * Returns all weapons the player currently carries.
			 */
			std::map<int, WeaponState> &getWeapons();

			/**
			 * Returns an array with all players.
			 */
			static std::vector<Player*> &getPlayers();

			/**
			 * Moves the player and executes other actions depending on the
			 * keyboard status.
			 */
			void think();
		private:
			int id;

			uint8_t keys;
			float rotation;

			Vector2F position;

			Client *owner;

			int hitpoints;
			int team;

			int lastweaponid;
			std::map<int, WeaponState> weapons;
			int currentweapon;

			static std::vector<Player*> players;

			static int lastid;
	};

	typedef SharedPointer<Player> PlayerPointer;
}

#endif
