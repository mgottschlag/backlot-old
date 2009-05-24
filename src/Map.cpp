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
		compiled = false;
		batches = 0;
		accessible = 0;
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
		if (batches)
			delete[] batches;
		if(accessible)
			delete[] accessible;
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
		if (!root->Attribute("size"))
		{
			std::cerr << "Missing map size." << std::endl;
			return false;
		}
		size = root->Attribute("size");
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
			std::string tilesetname = source.substr(0, source.find("."));
			if (tilesets.find(tilesetname) == tilesets.end())
			{
				std::cerr << "Tile set " << tilesetname << " not loaded." << std::endl;
				return false;
			}
			TileSetPointer set = tilesets.find(tilesetname)->second;
			std::string tilename = source.substr(source.find(".") + 1);
			Tile *tileinfo = set->getTile(tilename);
			if (!tileinfo)
			{
				std::cerr << "Tile " << tilename << " not loaded." << std::endl;
				return false;
			}
			tiledefs.insert(std::pair<int, Tile*>(id, tileinfo));
			tiledefnode = node->IterateChildren("tiledef", tiledefnode);
		}
		// Load tiles
		TiXmlNode *tilenode = root->FirstChild("tile");
		while (tilenode)
		{
			TiXmlElement *tile = tilenode->ToElement();
			if (!tile)
				continue;
			if (!tile->Attribute("id") || !tile->Attribute("position"))
			{
				std::cerr << "Missing tile id or position." << std::endl;
				return false;
			}
			TileInfo tileinfo;
			tileinfo.position = tile->Attribute("position");
			int id = atoi(tile->Attribute("id"));
			if (tiledefs.find(id) == tiledefs.end())
			{
				std::cerr << "Invalid tile definition id." << std::endl;
				return false;
			}
			tileinfo.tiledef = tiledefs.find(id)->second;
			tiles.push_back(tileinfo);
			tilenode = node->IterateChildren("tile", tilenode);
		}
		// Add to loaded maps
		this->name = name;
		compiled = false;
		maps.insert(std::pair<std::string, Map*>(name, this));
		return true;
	}

	bool Map::compile()
	{
		if (compiled)
			return true;
		// Create accessibility map
		createAccessibilityMap();
		// Get maximum layer count and collect textures
		getLayerCount();
		collectTextures();
		// Compute batch count
		int batchcountx = (size.x + 63) / 64;
		int batchcounty = (size.y + 63) / 64;
		// One solid and one shadow pass per texture
		int materialcount = textures.size() * 2;
		// Create render batches
		batches = new QuadBatchPointer[batchcountx * batchcounty * materialcount];
		for (int i = 0; i < batchcountx * batchcounty * materialcount; i++)
		{
			batches[i] = new QuadBatch();
		}
		// Fill batches
		std::list<TileInfo>::iterator it = tiles.begin();
		while (it != tiles.end())
		{
			const std::vector<QuadInfo> &quads = it->tiledef->getQuads();
			TileSet *tileset = it->tiledef->getTileSet();
			// Get texture
			int textureindex = 0;
			for (unsigned int i = 0; i < textures.size(); i++)
			{
				if (it->tiledef->getTexture() == textures[i])
				{
					textureindex = i;
					break;
				}
			}
			// Get layer counts
			int ground;
			int shadow;
			int high;
			tileset->getLayerCount(ground, shadow, high);
			// Get batches
			Vector2I position = it->position;
			int batchindexx = position.x / 64;
			if (batchindexx < 0)
				batchindexx = 0;
			if (batchindexx >= batchcountx)
				batchindexx = batchcountx - 1;
			int batchindexy = position.y / 64;
			if (batchindexy < 0)
				batchindexy = 0;
			if (batchindexy >= batchcounty)
				batchindexy = batchcounty - 1;
			int batchindex = textureindex * 2 * batchcountx * batchcounty
				+ batchindexx + batchindexy * batchcountx;
			QuadBatchPointer normalbatch = batches[batchindex];
			batchindex += batchcountx * batchcounty;
			QuadBatchPointer shadowbatch = batches[batchindex];
			// Add quads
			for (unsigned int i = 0; i < quads.size(); i++)
			{
				int layer = quads[i].layer;
				Quad quad;
				quad.position.x = (float)position.x + (float)quads[i].offset.x / 32;
				quad.position.y = (float)position.y + (float)quads[i].offset.y / 32;
				quad.size.x = (float)quads[i].texturerect.width / 32;
				quad.size.y = (float)quads[i].texturerect.height / 32;
				quad.texturecoords.x = (float)quads[i].texturerect.x / 128;
				quad.texturecoords.y = (float)quads[i].texturerect.y / 128;
				quad.texturecoords.width = (float)quads[i].texturerect.width / 128;
				quad.texturecoords.height = (float)quads[i].texturerect.height / 128;
				if (layer >= ground && layer < ground + shadow)
				{
					// Shadow layer
					float depth = layer - ground + groundlayers;
					quad.depth = depth;
					shadowbatch->addQuad(quad);
				}
				else
				{
					// Normal layer
					float depth = layer;
					if (layer >= groundlayers)
						depth = layer - ground - shadow + groundlayers + shadowlayers;
					quad.depth = depth;
					normalbatch->addQuad(quad);
				}
			}
			it++;
		}
		// Rebuild batches
		for (int i = 0; i < batchcountx * batchcounty * materialcount; i++)
		{
			batches[i]->rebuild();
		}
		compiled = true;
		return true;
	}
	bool Map::isCompiled()
	{
		return compiled;
	}
	bool Map::saveCompiled()
	{
		return false;
	}

	void Map::createAccessibilityMap()
	{
		accessible = new char[size.x * size.y];
		memset(accessible, 0, size.x * size.y);
		// Mark tiles as accessible
		std::list<TileInfo>::iterator it = tiles.begin();
		while (it != tiles.end())
		{
			if (it->tiledef->isAccessible())
			{
				const Vector2I &tilesize = it->tiledef->getSize();
				for (int y = 0; y < tilesize.y / 32; y++)
				{
					for (int x = 0; x < tilesize.x / 32; x++)
					{
						accessible[(y + it->position.y) * size.x + x + it->position.x] = 1;
					}
				}
			}
			it++;
		}
	}
	void Map::getLayerCount()
	{
		groundlayers = 0;
		shadowlayers = 0;
		highlayers = 0;
		std::map<std::string, TileSetPointer>::iterator it = tilesets.begin();
		while (it != tilesets.end())
		{
			int ground;
			int shadow;
			int high;
			it->second->getLayerCount(ground, shadow, high);
			if (groundlayers < ground)
				groundlayers = ground;
			if (shadowlayers < shadow)
				shadowlayers = shadow;
			if (highlayers < high)
				highlayers = high;
			it++;
		}
	}
	void Map::collectTextures()
	{
		textures.clear();
		std::map<std::string, TileSetPointer>::iterator it = tilesets.begin();
		while (it != tilesets.end())
		{
			textures.push_back(it->second->getTexture());
			it++;
		}
	}

	std::map<std::string, Map*> Map::maps;
}
