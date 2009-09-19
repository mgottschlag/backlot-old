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
#ifdef SERVER
#include "Game.hpp"
#endif

#include <iostream>
#include <fstream>
#include <GL/gl.h>

namespace backlot
{
	Map::Map() : ReferenceCounted()
	{
		heightmap = 0;
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
		// Remove entity states
		#ifdef SERVER
		entities.clear();
		#endif
		// Destroy map
		if (heightmap)
			delete[] heightmap;
		if (accessible)
			delete[] accessible;
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
		// TODO
		this->name = "";
		// Open file
		std::fstream file((Engine::get().getGameDirectory() + "/maps/" + name + ".blc").c_str(),
			std::ofstream::in | std::ofstream::binary);
		if (!file)
		{
			std::cerr << "Could not open map file " << name << ".blc." << std::endl;
			return false;
			
		}
		// Read header
		unsigned int version = 0;
		file.read((char*)&version, 4);
		if (version != MAP_FORMAT_VERSION)
		{
			std::cerr << "Wrong map version (" << version << " vs " << MAP_FORMAT_VERSION << ")." << std::endl;
			return false;
		}
		file.read((char*)&size.x, 4);
		file.read((char*)&size.y, 4);
		// Read height info
		heightmap = new float[size.x * size.y];
		int read = 0;
		while (read < size.x * size.y)
		{
			unsigned short runlength = 0;
			file.read((char*)&runlength, 2);
			if (!runlength)
			{
				std::cerr << "Error while reading height data." << std::endl;
				delete[] heightmap;
				heightmap = 0;
				return false;
			}
			float height = 0;
			file.read((char*)&height, 4);
			for (unsigned int i = 0; i < runlength; i++)
				heightmap[read + i] = height;
			read += runlength;
		}
		// Read accessibility info
		accessible = new unsigned char[(size.x * size.y + 1) / 2];
		file.read((char*)accessible, (size.x * size.y + 1) / 2);
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
			#ifdef SERVER
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
			#endif
			// TODO: Read properties
			unsigned int propertycount = 0;
			file.read((char*)&propertycount, 4);
		}
		#ifdef CLIENT
		// Read graphics
		unsigned int materialcount = 0;
		file.read((char*)&materialcount, 4);
		for (unsigned int i = 0; i < materialcount; i++)
		{
			MapLayer layer;
			// Read texture name
			unsigned short namelength = 0;
			file.read((char*)&namelength, 2);
			char *namedata = new char[namelength + 1];
			file.read(namedata, namelength);
			namedata[namelength] = 0;
			std::string texturename = std::string("tilesets/") + namedata + ".png";
			delete[] namedata;
			layer.texture = Texture::get(texturename);
			// Material flags
			unsigned char shadow = 0;
			file.read((char*)&shadow, 1);
			layer.shadow = shadow;
			// Read quad lists
			unsigned int batchcount = 0;
			file.read((char*)&batchcount, 4);
			for (unsigned int j = 0; j < batchcount; j++)
			{
				RectangleF rect;
				file.read((char*)&rect.x, 4);
				file.read((char*)&rect.y, 4);
				file.read((char*)&rect.width, 4);
				file.read((char*)&rect.height, 4);
				unsigned int vertexcount = 0;
				file.read((char*)&vertexcount, 4);
				float *vertexdata = new float[vertexcount * 5];
				file.read((char*)vertexdata, vertexcount * 5 * 4);
				layer.batches.push_back(new QuadBatch(vertexcount / 4, vertexdata));
			}
			layers.push_back(layer);
		}
		#endif
		// Add to loaded maps
		this->name = name;
		maps.insert(std::pair<std::string, Map*>(name, this));
		return true;
	}

	Vector2I Map::getSize()
	{
		return size;
	}
	unsigned char *Map::getPathFindingInfo()
	{
		return accessible;
	}

	float Map::getHeight(Vector2F position)
	{
		Vector2I ipos = position;
		if (ipos.x < 0 || ipos.y < 0)
			return 1000;
		if (ipos.x >= size.x || ipos.y >= size.y)
			return 1000;
		return heightmap[ipos.x + ipos.y * size.x];
	}
	float Map::getMaximumHeight(RectangleF area)
	{
		// Align area to integer boundaries
		Vector2I min(floor(area.x), floor(area.y));
		Vector2I max(ceil(area.x + area.width), ceil(area.y + area.height));
		// Loop through all squares
		float maxheight = -1000;
		for (int y = min.y; y < max.y; y++)
		{
			for (int x = min.x; x < max.x; x++)
			{
				float height = getHeight(Vector2F(x, y));
				if (height > maxheight)
					maxheight = height;
			}
		}
		return maxheight;
	}
	float Map::getMinimumHeight(RectangleF area)
	{
		// Align area to integer boundaries
		Vector2I min(floor(area.x), floor(area.y));
		Vector2I max(ceil(area.x + area.width), ceil(area.y + area.height));
		// Loop through all squares
		float minheight = 1000;
		for (int y = min.y; y < max.y; y++)
		{
			for (int x = min.x; x < max.x; x++)
			{
				float height = getHeight(Vector2F(x, y));
				if (height < minheight)
					minheight = height;
			}
		}
		return minheight;
	}

	bool Map::isAccessible(Vector2F start, Vector2F end, float maxheight,
		Vector2F *collision)
	{
		// Special cases
		if (start.x == end.x)
		{
			// TODO
			return true;
		}
		if (start.y == end.y)
		{
			// TODO
			return true;
		}
		// Go through the grid along the vector
		int startx = (int)start.x;
		int starty = (int)start.y;
		int dirx = (end.x > start.x) ? 1 : -1;
		int diry = (end.y > start.y) ? 1 : -1;
		int nextx = (int)((dirx == 1) ? floor(start.x + 1.001) : ceil(start.x - 1.001));
		int nexty = (int)((diry == 1) ? floor(start.y + 1.001) : ceil(start.y - 1.001));
		float nextx_y = start.y + (start.x - nextx) / (end.x - start.x) * (end.y - start.y);
		float nexty_x = start.x + (start.y - nexty) / (end.y - start.y) * (end.x - start.x);
		float tmaxxsquared = (start.x - nextx) * (start.x - nextx) + (start.y - nextx_y) * (start.y - nextx_y);
		float tmaxysquared = (start.y - nexty) * (start.y - nexty) + (start.x - nexty_x) * (start.x - nexty_x);
		float lengthsquared = (end - start).getLengthSquared();
		if (tmaxxsquared >= lengthsquared && tmaxysquared >= lengthsquared)
		{
			// End reached
			return true;
		}
		if (tmaxxsquared < tmaxysquared)
		{
			int nextx = startx + dirx;
			float nextheight = getHeight(Vector2F(nextx, starty));
			if (nextheight > maxheight)
			{
				// Collision
				// TODO: Collision info
				return false;
			}
			else
			{
				// Continue search
				// TODO: Can be optimized.
				return isAccessible(Vector2F((float)nextx, nextx_y), end, maxheight, collision);
			}
		}
		else
		{
			int nexty = starty + diry;
			float nextheight = getHeight(Vector2F(startx, nexty));
			if (nextheight > maxheight)
			{
				// Collision
				// TODO: Collision info
				return false;
			}
			else
			{
				// Continue search
				// TODO: Can be optimized.
				return isAccessible(Vector2F(nexty_x, (float)nexty), end, maxheight, collision);
			}
		}
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
		// Render normal layers
		glEnable(GL_TEXTURE_2D);
		for (unsigned int i = 0; i < layers.size(); i++)
		{
			if (!layers[i].shadow)
			{
				if (layers[i].texture)
					layers[i].texture->bind();
				for (unsigned int j = 0; j < layers[i].batches.size(); j++)
				{
					layers[i].batches[j]->render();
				}
			}
		}
		// Render shadow layers
		glEnable(GL_BLEND);
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		for (unsigned int i = 0; i < layers.size(); i++)
		{
			if (layers[i].shadow)
			{
				if (layers[i].texture)
					layers[i].texture->bind();
				for (unsigned int j = 0; j < layers[i].batches.size(); j++)
				{
					layers[i].batches[j]->render();
				}
			}
		}
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
	}
	#endif

	#ifdef SERVER
	void Map::loadEntities()
	{
		std::list<EntityStatePointer>::iterator it = entities.begin();
		while (it != entities.end())
		{
			Game::get().addEntityWithState((*it)->getTemplate()->getName(), 0,
				(*it)->get());
			it++;
		}
	}
	#endif

	#ifdef CLIENT
	Map *Map::visible = 0;
	#endif
	std::map<std::string, Map*> Map::maps;
}
