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
		// Destroy map
		if (heightmap)
			delete[] heightmap;
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
		// TODO
		// Read entities
		// TODO
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

	bool Map::isAccessible(RectangleF area)
	{
		return true;
	}
	bool Map::isAccessible(Vector2F start, Vector2F end, Vector2F *collision)
	{
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

	#ifdef CLIENT
	Map *Map::visible = 0;
	#endif
	std::map<std::string, Map*> Map::maps;
}
