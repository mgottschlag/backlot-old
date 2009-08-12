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

#include "Game.hpp"
#include "Engine.hpp"
#include "NetworkData.hpp"
#include "Server.hpp"
#include "Timer.hpp"

#include "support/tinyxml.h"

#include <iostream>

namespace backlot
{
	Game &Game::get()
	{
		static Game game;
		return game;
	}
	Game::~Game()
	{
	}

	bool Game::load(std::string mapname, int clientid)
	{
		this->clientid = clientid;
		this->mapname = mapname;
		for (int i = 0; i < 65535; i++)
			entities[i] = 0;
		time = 0;
		lag = 0;
		maxentityid = 0;
		return true;
	}
	bool Game::destroy()
	{
		// Remove entities
		for (int i = 0; i < maxentityid + 1; i++)
		{
			if (entities[i])
				entities[i]->destroyScript();
			entities[i] = 0;
		}
		return true;
	}

	EntityPointer Game::addEntity(std::string type, int owner, int id,
		BufferPointer state)
	{
		std::cout << "addEntity" << std::endl;
		// Get entity template
		EntityTemplatePointer tpl = EntityTemplate::get(type);
		if (tpl.isNull())
		{
			std::cerr << "Could not get entity template \"" << type << "\"." << std::endl;
			return 0;
		}
		// Create entity
		if (id > maxentityid)
			maxentityid = id;
		EntityPointer entity = new Entity();
		entity->setID(id);
		entity->setOwner(owner);
		entity->create(tpl, state);
		// Insert entity into list
		entities[id] = entity;
		if (owner == getClientID())
			localentities.push_back(entity);
		return entity;
	}
	void Game::removeEntity(EntityPointer entity)
	{
		int id = entity->getID();
		removeEntity(id);
	}
	void Game::removeEntity(unsigned int id)
	{
		if (id >= 65535)
			return;
		// Delete from local list
		if (entities[id]->isLocal())
		{
			// TODO: Maybe even use a std::vector?
			std::list<EntityPointer>::iterator it = localentities.begin();
			while (it != localentities.end())
			{
				if ((*it)->getID() == (int)id)
				{
					localentities.erase(it);
					break;
				}
				it++;
			}
		}
		// Delete entity
		entities[id]->destroyScript();
		entities[id] = 0;
	}
	EntityPointer Game::getEntity(int id)
	{
		if (id >= 65535)
			return 0;
		return entities[id];
	}

	int Game::getClientID()
	{
		return clientid;
	}

	unsigned int Game::getTime()
	{
		return time;
	}

	void Game::injectUpdates(BufferPointer buffer)
	{
		// Get server time step to which this update belongs to
		unsigned int updatetime = buffer->read32();
		lag = buffer->read32();
		time = updatetime;
		while (1)
		{
			// Get entity
			int entityid = buffer->read16();
			if (!entityid)
				break;
			entityid--;
			if (entities[entityid].isNull())
			{
				std::cout << "Entity " << entityid << " not available." << std::endl;
				return;
			}
			EntityPointer entity = entities[entityid];
			// Apply update
			entity->applyUpdate(buffer, lag);
		}
		// Ack updates.
		BufferPointer received = new Buffer();
		received->write8(EPT_UpdateReceived);
		received->write32(updatetime);
		Client::get().send(received);
	}

	void Game::setAcknowledgedPacket(int time)
	{
		// Clean up prediction data
		for (int i = 0; i < maxentityid + 1; i++)
		{
			if (entities[i])
				entities[i]->dropPredictionData(time);
		}
	}
	void Game::setLag(unsigned int lag)
	{
		//this->lag = lag;
	}

	void Game::setInputTarget(EntityPointer entity)
	{
		inputentity = entity;
	}
	EntityPointer Game::getInputTarget()
	{
		return inputentity.get();
	}

	CollisionInfo Game::getCollision(Vector2F from, Vector2F to,
		float maxheight)
	{
		CollisionInfo collision;
		collision.collision = false;
		// Check map collision
		if (!Client::get().getMap()->isAccessible(from, to, maxheight,
			&collision.point))
		{
			collision.collision = true;
		}
		return collision;
	}
	EntityListPointer Game::getEntities(RectangleF area, std::string type)
	{
		EntityListPointer list = new EntityList();
		for (int i = 0; i < maxentityid + 1; i++)
		{
			if (entities[i] && entities[i]->isMovable())
			{
				// Check type if necessary
				if (type != "" &&  entities[i]->getTemplate()->getName() != type)
					continue;
				// Check bounding rectangle
				RectangleF br = entities[i]->getRectangle();
				if (br.overlapsWith(area))
					list->addEntity(entities[i]);
			}
		}
		return list;
	}
	EntityListPointer Game::getEntities(RectangleF area)
	{
		return getEntities(area, "");
	}
	EntityListPointer Game::getEntities(std::string type)
	{
		EntityListPointer list = new EntityList();
		for (int i = 0; i < maxentityid + 1; i++)
		{
			if (entities[i] && entities[i]->getTemplate()->getName() == type)
			{
				list->addEntity(entities[i]);
			}
		}
		return list;
	}

	void Game::update()
	{
		// Update entities
		for (int i = 0; i < maxentityid + 1; i++)
		{
			if (entities[i])
				entities[i]->update();
		}
		// Timer callbacks
		Timer::callCallbacks();
		// Increase tick counter
		time++;
		// Send updates to the server
		int from = Client::get().getAcknowledgedPacket();
		BufferPointer buffer = new Buffer();
		buffer->write8(EPT_Update);
		buffer->write32(time);
		unsigned int updatecount = 0;
		// Check all entities
		std::list<EntityPointer>::iterator it = localentities.begin();
		while (it != localentities.end())
		{
			// Add update to the packet
			if ((*it)->hasChanged(from))
			{
				updatecount++;
				buffer->write16((*it)->getID() + 1);
				(*it)->getUpdate(from, buffer);
			}
			it++;
		}
		// Send updates
		if (updatecount > 0)
			Client::get().send(buffer);
	}

	Game::Game()
	{
	}
}
