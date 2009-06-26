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
		return true;
	}
	bool Game::destroy()
	{
		return false;
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
		EntityPointer entity = new Entity();
		entity->setOwner(owner);
		entity->create(tpl, state);
		// Insert entity into list
		entities[id] = entity;
		return entity;
	}
	void Game::removeEntity(EntityPointer entity)
	{
		// Look for entity to delete
		for (int i = 0; i < 65535; i++)
		{
			if (entities[i] == entity)
			{
				// Delete entity
				entities[i] = 0;
				break;
			}
		}
	}
	EntityPointer Game::getEntity(int id)
	{
		if (id >= 65535)
			return 0;
		return entities[id];
	}
	std::vector<EntityPointer> Game::getEntities(std::string type)
	{
		std::vector<EntityPointer> entitylist;
		for (int i = 0; i < 65535; i++)
		{
			if (entities[i] && entities[i]->getTemplate()->getName() == type)
			{
				entitylist.push_back(entities[i]);
			}
		}
		return std::vector<EntityPointer>();
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
		std::cout << "Got updates." << std::endl;
		// Get server time step to which this update belongs to
		unsigned int updatetime = buffer->read32();
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
			std::cout << "Updating entity." << std::endl;
			entity->applyUpdate(buffer);
			break;
		}
		// Ack updates.
		BufferPointer received = new Buffer();
		received->write8(EPT_UpdateReceived);
		received->write32(updatetime);
		Client::get().send(received);
	}

	void Game::update()
	{
		// Update entities
		for (int i = 0; i < 65535; i++)
		{
			entities[i]->update();
		}
		// Increase tick counter
		time++;
		// Send updates
		// TODO
	}

	Game::Game()
	{
	}
}
