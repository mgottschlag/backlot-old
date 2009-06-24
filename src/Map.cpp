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
#include <GL/gl.h>
#include <cmath>

namespace backlot
{
	Map::Map() : ReferenceCounted()
	{
		compiled = false;
		#ifdef CLIENT
		batches = 0;
		#endif
		accessible = 0;
	}
	Map::~Map()
	{
		#ifdef CLIENT
		// Hide
		if (isVisible())
			setVisible(false);
		#endif
		// Remove from loaded maps
		std::map<std::string, Map*>::iterator it = maps.find(name);
		if (it != maps.end())
		{
			maps.erase(it);
		}
		// Destroy map
		#ifdef CLIENT
		if (batches)
			delete[] batches;
		#endif
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
		#ifdef CLIENT
		// Get maximum layer count and collect textures
		getLayerCount();
		collectTextures();
		// Compute batch count
		batchcountx = (size.x + 63) / 64;
		batchcounty = (size.y + 63) / 64;
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
			const Vector2I &texsize = textures[textureindex]->getSize();
			// Add quads
			for (unsigned int i = 0; i < quads.size(); i++)
			{
				int layer = quads[i].layer;
				Quad quad;
				quad.position.x = (float)position.x + (float)quads[i].offset.x / 32;
				quad.position.y = (float)position.y + (float)quads[i].offset.y / 32;
				quad.size.x = (float)quads[i].texturerect.width / 32;
				quad.size.y = (float)quads[i].texturerect.height / 32;
				quad.texturecoords.x = (float)quads[i].texturerect.x / texsize.x;
				quad.texturecoords.y = (float)quads[i].texturerect.y / texsize.y;
				quad.texturecoords.width = (float)quads[i].texturerect.width / texsize.x;
				quad.texturecoords.height = (float)quads[i].texturerect.height / texsize.y;
				quad.rotated = quads[i].rotated;
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
		#endif
		compiled = true;
		return true;
	}
	bool Map::isCompiled()
	{
		return compiled;
	}

	bool Map::isAccessible(RectangleF area)
	{
		// Get area
		int x1 = (int)floor(area.x);
		int y1 = (int)floor(area.y);
		int x2 = (int)ceil(area.x + area.width);
		int y2 = (int)ceil(area.y + area.height);
		// Check
		if (x1 < 0 || y1 < 0 || x2 > size.x || y2 > size.y)
			return false;
		for (int y = y1; y < y2; y++)
		{
			for (int x = x1; x < x2; x++)
			{
				if (!accessible[y * size.x + x])
					return false;
			}
		}
		return true;
	}
	bool Map::isAccessible(Vector2F start, Vector2F end, Vector2F *collision)
	{
		// TODO: Compute collision point
		// Area to be checked
		RectangleI area((int)floor(start.x), (int)floor(start.y), (int)(floor(end.x) - floor(start.x)), (int)(floor(end.y) - floor(start.y)));
		if (area.width < 0)
		{
			area.x = (int)floor(end.x);
			area.width = (int)-area.width;
		}
		if (area.height < 0)
		{
			area.y = (int)floor(end.y);
			area.height = (int)-area.height;
		}
		int hitx = -10;
		int hity = -10;
		float hitdistance = area.width * area.height + 100;
		// Loop through all tiles
		for (int py = area.y; py <= area.y + area.height; py++)
		{
			// FIXME: This is ugly. We need just half as much code as this.
			if (py < 0 || py >= size.y)
			{
				if (!collision)
					return false;
				else
				{
					for (int px = area.x; px <= area.x + area.width; px++)
					{
						// Compute distance from start
						float distance = sqrt((start.x - px) * (start.x - px) + (start.y - py) * (start.y - py));
						if (distance < hitdistance)
						{
							hitx = px;
							hity = py;
						}
					}
				}
			}
			for (int px = area.x; px <= area.x + area.width; px++)
			{
				if (px < 0 || px >= size.x)
				{
					if (!collision)
						return false;
					else
					{
						// Compute distance from start
						float distance = sqrt((start.x - px) * (start.x - px) + (start.y - py) * (start.y - py));
						if (distance < hitdistance)
						{
							hitx = px;
							hity = py;
						}
					}
				}
				// Check whether tile collides with line
				if (end.x != start.x)
				{
					float dx = (end.y - start.y) / (end.x - start.x);
					float n = start.y - start.x * dx;
					bool above0 = (py > (px * dx + n));
					bool above1 = (py + 1 > (px * dx + n));
					bool above2 = (py > ((px + 1) * dx + n));
					bool above3 = (py + 1 > ((px + 1) * dx + n));
					if ((above0 != above1)
						|| (above1 != above2)
						|| (above2 != above3))
					{
						// Check tile
						if (!accessible[py * size.x + px])
						{
							if (!collision)
								return false;
							else
							{
								// Compute distance from start
								float distance = sqrt((start.x - px) * (start.x - px) + (start.y - py) * (start.y - py));
								if (distance < hitdistance)
								{
									hitx = px;
									hity = py;
								}
							}
						}
					}
				}
				else
				{
					float dy = (end.x - start.x) / (end.y - start.y);
					float n = start.x - start.y * dy;
					bool above0 = (px > (py * dy + n));
					bool above1 = (px + 1 > (py * dy + n));
					bool above2 = (px > ((py + 1) * dy + n));
					bool above3 = (px + 1 > ((py + 1) * dy + n));
					if ((above0 != above1)
						|| (above1 != above2)
						|| (above2 != above3))
					{
						// Check tile
						if (!accessible[py * size.x + px])
						{
							if (!collision)
								return false;
							else
							{
								// Compute distance from start
								float distance = sqrt((start.x - px) * (start.x - px) + (start.y - py) * (start.y - py));
								if (distance < hitdistance)
								{
									hitx = px;
									hity = py;
								}
							}
						}
					}
				}
			}
		}
		if (collision)
		{
			if (hitx == -10)
				return true;
			else
			{
				// Compute hit point
				RectangleF tile(hitx, hity, 1, 1);
				Vector2F intersection1;
				Vector2F intersection2;
				if (!tile.getIntersections(Line(start, end), intersection1,
					intersection2))
				{
					*collision = Vector2F(hitx, hity);
					std::cerr << "Error: Wrong intersection detection." << std::endl;
					return false;
				}
				*collision = intersection1;
				return false;
			}
		}
		else
			return true;
	}

	#ifdef CLIENT
	void Map::setVisible(bool visible)
	{
		if (visible)
			Map::visible = this;
		else if (Map::visible == this)
			Map::visible = 0;
	}
	bool Map::isVisible()
	{
		return visible == this;
	}
	SharedPointer<Map> Map::getVisibleMap()
	{
		return visible;
	}

	void Map::render()
	{
		if (!compiled)
		{
			std::cout << "Map not compiled." << std::endl;
			return;
		}
		// Normal layers
		glEnable(GL_TEXTURE_2D);
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glColor3f(1.0, 1.0, 1.0);
		for (unsigned int tex = 0; tex < textures.size(); tex++)
		{
			textures[tex]->bind();
			for (int i = 0; i < batchcountx * batchcounty; i++)
			{
				batches[tex * 2 * batchcountx * batchcounty + i]->render();
			}
		}
		// Shadow layers
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		for (unsigned int tex = 0; tex < textures.size(); tex++)
		{
			textures[tex]->bind();
			for (int i = 0; i < batchcountx * batchcounty; i++)
			{
				batches[(tex * 2 + 1) * batchcountx * batchcounty + i]->render();
			}
		}
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	#endif

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
	#ifdef CLIENT
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

	Map *Map::visible = 0;
	#endif
	std::map<std::string, Map*> Map::maps;
}
