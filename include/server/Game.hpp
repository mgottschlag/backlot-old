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

#ifndef _GAME_HPP_
#define _GAME_HPP_

#include "Script.hpp"
#include "entity/Entity.hpp"
#include "entity/EntityList.hpp"
#include "Client.hpp"
#include "Rectangle.hpp"

#include <queue>

namespace backlot
{
	struct CollisionInfo
	{
		bool collision;
		bool entitycollision;
		EntityPointer entity;
		Vector2F point;
	};

	class Game
	{
		public:
			static Game &get();
			~Game();

			bool load(std::string mapname, std::string mode);
			bool destroy();

			std::string getMode();
			int getTeamCount();
			int getWeaponSlotCount();
			float getPlayerSpeed();

			EntityPointer addEntity(std::string type, int owner = 0);
			EntityPointer addEntityWithState(std::string type, int owner,
				BufferPointer state);
			void removeEntity(EntityPointer entity);
			EntityPointer getEntity(int id);
			void registerForDeletion(int id);

			bool onClientConnecting(Client *client);
			void addClient(Client *client);
			void removeClient(Client *client);

			/**
			 * Returns the number of ticks passed since the start of the game.
			 */
			unsigned int getTime();

			void injectUpdates(Client *client, BufferPointer buffer);

			CollisionInfo getCollision(Vector2F from, Vector2F to,
				float maxheight);
			EntityListPointer getEntities(RectangleF area, std::string type);
			EntityListPointer getEntities(RectangleF area);
			EntityListPointer getEntities(std::string type);

			void update();
		private:
			Game();

			std::string mode;
			int teamcount;
			int weaponslots;
			float speed;
			ScriptPointer script;
			std::string mapname;

			EntityPointer entities[65535];
			int maxentityid;
			std::queue<int> deletionqueue;

			std::map<int, Client*> clients;
			int lastclientid;

			unsigned int time;
	};
}

#endif
