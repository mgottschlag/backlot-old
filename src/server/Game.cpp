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

	bool Game::load(std::string mapname, std::string mode)
	{
		lastclientid = 0;
		this->mapname = mapname;
		for (int i = 0; i < 65535; i++)
			entities[i] = 0;
		maxentityid = 0;
		// Open XML file
		std::string filename = Engine::get().getGameDirectory() + "/modes/" + mode + ".xml";
		TiXmlDocument xml(filename.c_str());
		if (!xml.LoadFile() || xml.Error())
		{
			std::cerr << "Could not load XML file " << mode << ".xml: " << xml.ErrorDesc() << std::endl;
			return false;
		}
		TiXmlNode *node = xml.FirstChild("mode");
		if (!node)
		{
			std::cerr << "Parser error: <mode> not found." << std::endl;
			return false;
		}
		TiXmlElement *root = node->ToElement();
		// Get mode attributes
		teamcount = 0;
		root->Attribute("teams", &teamcount);
		// Read player setup
		TiXmlElement *playerinfo = 0;
		TiXmlNode *playernode = root->FirstChild("playerinfo");
		while (playernode)
		{
			playerinfo = playernode->ToElement();
			if (playerinfo)
				break;
			playernode = node->IterateChildren("playerinfo", playernode);
		}
		if (!playerinfo)
		{
			std::cerr << "No player info found!" << std::endl;
			return false;
		}
		double speed = 1.0;
		if (playerinfo->Attribute("speed"))
		{
			playerinfo->Attribute("speed", &speed);
		}
		this->speed = speed;
		weaponslots = 10;
		if (playerinfo->Attribute("weaponslots"))
		{
			playerinfo->Attribute("weaponslots", &weaponslots);
		}
		// Load script
		script = new Script();
		script->addCoreFunctions();
		script->addServerFunctions();
		// TODO: Other functions
		TiXmlNode *scriptnode = root->FirstChild("script");
		while (scriptnode)
		{
			TiXmlElement *scriptdata = scriptnode->ToElement();
			std::string code;
			if (scriptdata)
			{
				TiXmlNode *textnode = scriptdata->FirstChild();
				while (textnode)
				{
					TiXmlText *text = textnode->ToText();
					if (text && text->Value())
					{
						code += text->Value();
					}
					textnode = scriptdata->IterateChildren(textnode);
				}
			}
			script->runString(code);
			scriptnode = node->IterateChildren("script", scriptnode);
		}
		this->mode = mode;
		time = 0;
		return true;
	}
	bool Game::destroy()
	{
		clients.clear();
		script = 0;
		return false;
	}

	int Game::getTeamCount()
	{
		return teamcount;
	}
	int Game::getWeaponSlotCount()
	{
		return weaponslots;
	}
	float Game::getPlayerSpeed()
	{
		return speed;
	}

	EntityPointer Game::addEntity(std::string type, int owner)
	{
		return addEntityWithState(type, owner, 0);
	}
	EntityPointer Game::addEntityWithState(std::string type, int owner,
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
		// Get new entity id
		int newindex = 65535;
		for (int id = 0; id < 65535; id++)
		{
			if (entities[id].isNull())
			{
				newindex = id;
				break;
			}
		}
		if (newindex == 65535)
		{
			std::cerr << "Too many entities." << std::endl;
			return 0;
		}
		if (newindex > maxentityid)
			maxentityid = newindex;
		// Create entity
		EntityPointer entity = new Entity();
		entity->setID(newindex);
		entity->setOwner(owner);
		entity->create(tpl, state);
		// Set owner
		// TODO
		// Insert entity into list
		entities[newindex] = entity;
		// Send entity to all connected clients
		BufferPointer buffer = new Buffer();
		buffer->write8(EPT_EntityCreated);
		buffer->write16(newindex);
		buffer->write16(owner);
		buffer->writeString(type);
		entity->getState(buffer);
		Server::get().sendToAll(buffer, true);
		// Set to active on all clients
		std::map<int, Client*>::iterator it = clients.begin();
		while (it != clients.end())
		{
			it->second->setEntityActive(newindex, true);
			it++;
		}
		return entity;
	}
	void Game::removeEntity(EntityPointer entity)
	{
		// Look for entity to delete
		for (int i = 0; i < maxentityid + 1; i++)
		{
			if (entities[i] == entity)
			{
				// Send message to all connected clients
				BufferPointer buffer = new Buffer();
				buffer->write8(EPT_EntityDeleted);
				buffer->write16(i);
				Server::get().sendToAll(buffer, true);
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
	void Game::registerForDeletion(int id)
	{
		deletionqueue.push(id);
	}

	bool Game::onClientConnecting(Client *client)
	{
		// Create packet with initial world state
		BufferPointer msg = new Buffer;
		msg->write8(EPT_InitialData);
		// Add map name
		msg->writeString(mapname);
		// Client ID
		int id = ++lastclientid;
		msg->write16(id);
		client->setID(id);
		// Send packet
		client->send(msg, true);
		return true;
	}
	void Game::addClient(Client *client)
	{
		std::cout << "New client." << std::endl;
		// Add to client list
		clients.insert(std::pair<int, Client*>(client->getID(), client));
		// Send entities
		for (int i = 0; i < maxentityid + 1; i++)
		{
			if (entities[i])
			{
				BufferPointer buffer = new Buffer();
				buffer->write8(EPT_EntityCreated);
				buffer->write16(i);
				buffer->write16(entities[i]->getOwner());
				buffer->writeString(entities[i]->getTemplate()->getName());
				entities[i]->getState(buffer);
				client->send(buffer, true);
			}
		}
		// Script callback
		if (script->isFunction("on_new_client"))
		{
			script->callFunction("on_new_client", lastclientid);
		}
	}
	void Game::removeClient(Client *client)
	{
		int clientid = -1;
		// Remove client
		std::map<int, Client*>::iterator it = clients.begin();
		while (it != clients.end())
		{
			if (client == it->second)
			{
				clientid = it->first;
				clients.erase(it);
				break;
			}
			it++;
		}
		if (clientid == -1)
			return;
		// Remove entities
		for (int i = 0; i < maxentityid + 1; i++)
		{
			if (entities[i] && entities[i]->getOwner() == clientid)
			{
				removeEntity(entities[i]);
			}
		}
	}

	unsigned int Game::getTime()
	{
		return time;
	}

	void Game::injectUpdates(Client *client, BufferPointer buffer)
	{
		// Get client time step to which this update belongs to
		unsigned int updatetime = buffer->read32();
		std::cout << "Client update: " << updatetime << std::endl;
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
			if (entity->getOwner() != client->getID())
				return;
			// Apply update
			std::cout << "Updating entity." << std::endl;
			entity->applyUpdate(buffer);
			break;
		}
		// Ack updates.
		BufferPointer received = new Buffer();
		received->write8(EPT_UpdateReceived);
		received->write32(updatetime);
		received->write16(time - updatetime);
		client->send(received);
		client->setLag(time - updatetime);
	}

	CollisionInfo Game::getCollision(Vector2F from, Vector2F to,
		float maxheight)
	{
		CollisionInfo collision;
		collision.collision = false;
		collision.entitycollision = false;
		collision.entity = 0;
		collision.point = Vector2F();
		// Check map collision
		if (!Server::get().getMap()->isAccessible(from, to, maxheight,
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
		// Increase tick counter
		time++;
		// Update entities
		for (int i = 0; i < maxentityid + 1; i++)
		{
			if (entities[i])
				entities[i]->update();
		}
		// Delete entities in the deletion queue
		while (deletionqueue.size() > 0)
		{
			int id = deletionqueue.front();
			deletionqueue.pop();
			removeEntity(getEntity(id));
		}
		// Timer callbacks
		Timer::callCallbacks();
		// Send updates to all clients
		std::map<int, Client*>::iterator it = clients.begin();
		while (it != clients.end())
		{
			Client *client = it->second;
			int from = client->getAcknowledgedPacket();
			std::cerr << "Time: " << from << " (currently: " << time << ")." << std::endl;
			BufferPointer buffer = new Buffer();
			buffer->write8(EPT_Update);
			buffer->write32(time);
			buffer->write32(client->getLag());
			// Check all entities
			for (int i = 0; i < maxentityid + 1; i++)
			{
				if (entities[i].isNull())
					continue;
				bool currentlyactive = client->isEntityActive(i);
				// TODO
				bool active = true;
				if (active)
				{
					if (!currentlyactive)
					{
						// Activate object
						// TODO
					}
					// Add update to the packet
					if (entities[i]->hasChanged(from))
					{
						buffer->write16(i + 1);
						entities[i]->getUpdate(from, buffer, it->first);
					}
				}
				else
				{
					if (currentlyactive)
					{
						// Deactivate object
						// TODO
					}
				}
			}
			// Send updates
			client->send(buffer);
			it++;
		}
	}

	Game::Game()
	{
	}
}
