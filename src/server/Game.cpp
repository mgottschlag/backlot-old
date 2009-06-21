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
		nextentity = 0;
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
		for (int i = 0; i < 65535; i++)
		{
			int id = (nextentity + i) % 65535;
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
		nextentity = (newindex + 1) % 65535;
		// Create entity
		EntityPointer entity = new Entity();
		entity->create(tpl);
		// Set owner
		// TODO
		// Insert entity into list
		entities[newindex] = entity;
		// Send entity to all connected clients
		// TODO
		return entity;
	}
	void Game::removeEntity(EntityPointer entity)
	{
		// Send message to all connected clients
		// TODO
		// Remove entity
		for (int i = 0; i < 65535; i++)
		{
			if (entities[i] == entity)
			{
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
		return std::vector<EntityPointer>();
	}

	bool Game::onClientConnecting(Client *client)
	{
		// Create packet with initial world state
		BufferPointer msg = new Buffer;
		msg->write8(EPT_InitialData);
		// Add map name
		msg->writeString(mapname);
		// Send packet
		client->send(msg, true);
		return true;
	}
	void Game::addClient(Client *client)
	{
		std::cout << "New client." << std::endl;
		// Add to client list
		clients.insert(std::pair<int, Client*>(++lastclientid, client));
		// Send entities
		// TODO
		// Script callback
		if (script->isFunction("on_new_client"))
		{
			script->callFunction("on_new_client", lastclientid);
		}
	}
	void Game::removeClient(Client *client)
	{
		
	}

	unsigned int Game::getTime()
	{
		return time;
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