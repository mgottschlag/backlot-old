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

#include "Map.hpp"
#include "Engine.hpp"

#include "tinyxml.h"

#include <iostream>

namespace backlot
{
	Map::Map() : ReferenceCounted()
	{
	}
	Map::~Map()
	{
		// Remove from loaded maps
		std::map<std::string, Map*>::iterator it = maps.find(name);
		if (it != maps.end())
		{
			maps.erase(it);
		}
		// Destroy map
		// TODO
	}

	SharedPointer<Map> Map::get(std::string name)
	{
		// Get already loaded map
		std::map<std::string, Map*>::iterator it = maps.find(name);
		if (it != maps.end())
		{
			return it->second;
		}
		// Load map
		MapPointer map = new Map();
		if (!map->load(name))
		{
			return 0;
		}
		return map;
	}
	bool Map::load(std::string name)
	{
		// Remove from loaded maps
		std::map<std::string, Map*>::iterator it = maps.find(name);
		if (it != maps.end())
		{
			maps.erase(it);
		}
		// Unload data
		this->name = name;
		tilesets.clear();
		tiledefs.clear();
		// Open XML file
		std::string filename = Engine::get().getGameDirectory() + "/maps/" + name + ".xml";
		TiXmlDocument xml(filename.c_str());
		if (!xml.LoadFile() || xml.Error())
		{
			std::cerr << "Could not load XML file " << name << ".xml: " << xml.ErrorDesc() << std::endl;
			return false;
		}
		TiXmlNode *node = xml.FirstChild("map");
		if (!node)
		{
			std::cerr << "Parser error: <map> not found." << std::endl;
			return false;
		}
		TiXmlElement *root = node->ToElement();
		// Load map information
		// TODO
		// Load tilesets
		TiXmlNode *tilesetnode = root->FirstChild("tileset");
		while (tilesetnode)
		{
			TiXmlElement *tileset = tilesetnode->ToElement();
			if (!tileset)
				continue;
			if (!tileset->Attribute("name"))
			{
				std::cerr << "Missing tileset name." << std::endl;
				return false;
			}
			std::string name = tileset->Attribute("name");
			TileSetPointer set = new TileSet();
			if (!set->load(name))
			{
				std::cerr << "Could not load tileset \"" << name << "\"." << std::endl;
				return false;
			}
			tilesets.insert(std::pair<std::string, TileSetPointer>(name, set));
			tilesetnode = node->IterateChildren("tileset", tilesetnode);
		}
		// Load tile definitions
		TiXmlNode *tiledefnode = root->FirstChild("tiledef");
		while (tiledefnode)
		{
			TiXmlElement *tiledef = tiledefnode->ToElement();
			if (!tiledef)
				continue;
			if (!tiledef->Attribute("id") || !tiledef->Attribute("source"))
			{
				std::cerr << "Missing tiledef id or source." << std::endl;
				return false;
			}
			int id = atoi(tiledef->Attribute("id"));
			std::string source = tiledef->Attribute("source");
			tiledefs.insert(std::pair<int, std::string>(id, source));
			tiledefnode = node->IterateChildren("tiledef", tiledefnode);
		}
		// Load tiles
		TiXmlNode *tilenode = root->FirstChild("tile");
		while (tilenode)
		{
			TiXmlElement *tile = tilenode->ToElement();
			if (!tile)
				continue;
			std::cout << "Tile." << std::endl;
			if (!tile->Attribute("id") || !tile->Attribute("position"))
			{
				std::cerr << "Missing tile id or position." << std::endl;
				return false;
			}
			tilenode = node->IterateChildren("tile", tilenode);
		}
		// Add to loaded maps
		this->name = name;
		maps.insert(std::pair<std::string, Map*>(name, this));
		return true;
	}

	bool Map::compile()
	{
		return false;
	}
	bool Map::isCompiled()
	{
		return false;
	}
	bool Map::saveCompiled()
	{
		return false;
	}

	std::map<std::string, Map*> Map::maps;
}
