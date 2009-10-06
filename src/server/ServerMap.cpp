/*
Copyright (c) 2009, Mathias Gottschlag

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "ServerMap.hpp"
#include "Engine.hpp"
#include "Game.hpp"

#include <iostream>
#include <fstream>

namespace backlot
{
	ServerMap::ServerMap() : Map()
	{
	}
	ServerMap::~ServerMap()
	{
		// Remove from loaded maps
		std::map<std::string, ServerMap*>::iterator it = maps.find(name);
		if (it != maps.end())
		{
			maps.erase(it);
		}
		// Remove entity states
		entities.clear();
	}

	SharedPointer<ServerMap> ServerMap::get(std::string name)
	{
		// Get already loaded map
		std::map<std::string, ServerMap*>::iterator it = maps.find(name);
		if (it != maps.end())
		{
			return it->second;
		}
		// Load map
		ServerMapPointer map = new ServerMap();
		if (!map->load(name))
		{
			return 0;
		}
		return map;
	}

	bool ServerMap::load(std::string name)
	{
		// Open file
		std::ifstream file((Engine::get().getGameDirectory() + "/maps/" + name + ".blc").c_str(),
			std::ifstream::in | std::ifstream::binary);
		if (!file)
		{
			std::cerr << "Could not open map file " << name << ".blc." << std::endl;
			return false;

		}
		// Read header and general information
		if (!readHeader(file))
			return false;
		// Read entities
		unsigned int entitycount = 0;
		file.read((char*)&entitycount, 4);
		std::cout << entitycount << " entities." << std::endl;
		for (unsigned int i = 0; i < entitycount; i++)
		{
			unsigned short namelength = 0;
			file.read((char*)&namelength, 2);
			char *namedata = new char[namelength + 1];
			file.read(namedata, namelength);
			namedata[namelength] = 0;
			std::string entityname = namedata;
			std::cout << "Entity: \"" << entityname << "\"" << std::endl;
			delete[] namedata;
			float x;
			float y;
			file.read((char*)&x, 4);
			file.read((char*)&y, 4);
			// Get template
			EntityTemplatePointer tpl = EntityTemplate::get(entityname);
			if (!tpl)
			{
				std::cout << "Invalid entity type in map: \"" << entityname
					<< "\"" << std::endl;
				return false;
			}
			EntityStatePointer state = new EntityState(tpl);
			entities.push_back(state);
			// Set position
			Property *position = state->getProperty("position");
			if (position)
				position->setVector2F(Vector2F(x, y));
			// TODO: Read properties
			unsigned int propertycount = 0;
			file.read((char*)&propertycount, 4);
		}
		// Add to loaded maps
		this->name = name;
		maps.insert(std::pair<std::string, ServerMap*>(name, this));
		return true;
	}

	void ServerMap::loadEntities()
	{
		std::list<EntityStatePointer>::iterator it = entities.begin();
		while (it != entities.end())
		{
			Game::get().addEntityWithState((*it)->getTemplate()->getName(), 0,
				(*it)->get());
			it++;
		}
	}

	std::map<std::string, ServerMap*> ServerMap::maps;
}
