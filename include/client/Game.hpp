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
#include "Client.hpp"

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

			bool load(std::string mapname, int clientid);
			bool destroy();

			EntityPointer addEntity(std::string type, int owner, int id,
				BufferPointer state = 0);
			void removeEntity(EntityPointer entity);
			void removeEntity(unsigned int id);
			EntityPointer getEntity(int id);
			std::vector<EntityPointer> getEntities(std::string type);

			int getClientID();

			/**
			 * Returns the number of ticks passed since the start of the game.
			 */
			unsigned int getTime();

			void injectUpdates(BufferPointer buffer);

			void setAcknowledgedPacket(int time);
			void setLag(unsigned int lag);

			void setInputTarget(EntityPointer entity);
			EntityPointer getInputTarget();

			CollisionInfo getCollision(Vector2F from, Vector2F to);

			void update();
		private:
			Game();

			int clientid;
			std::string mapname;

			EntityPointer entities[65535];
			WeakPointer<Entity> inputentity;

			unsigned int time;
			unsigned int lag;
	};
}

#endif
