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

#include "ClientMap.hpp"
#include "Engine.hpp"

#include <iostream>
#include <fstream>
#include <GL/gl.h>

namespace backlot
{
	ClientMap::ClientMap() : Map()
	{
	}
	ClientMap::~ClientMap()
	{
		// Hide
		if (isVisible())
			setVisible(false);
		// Remove from loaded maps
		std::map<std::string, ClientMap*>::iterator it = maps.find(name);
		if (it != maps.end())
		{
			maps.erase(it);
		}
	}

	SharedPointer<ClientMap> ClientMap::get(std::string name)
	{
		// Get already loaded map
		std::map<std::string, ClientMap*>::iterator it = maps.find(name);
		if (it != maps.end())
		{
			return it->second;
		}
		// Load map
		ClientMapPointer map = new ClientMap();
		if (!map->load(name))
		{
			return 0;
		}
		return map;
	}
	bool ClientMap::load(std::string name)
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
		maps.insert(std::pair<std::string, ClientMap*>(name, this));
		return true;
	}

	void ClientMap::setVisible(bool visible)
	{
		if (visible)
			ClientMap::visible = this;
		else if (ClientMap::visible == this)
			ClientMap::visible = 0;
	}
	bool ClientMap::isVisible()
	{
		return visible == this;
	}
	SharedPointer<ClientMap> ClientMap::getVisibleMap()
	{
		return visible;
	}

	void ClientMap::render()
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

	ClientMap *ClientMap::visible = 0;
	std::map<std::string, ClientMap*> ClientMap::maps;
}
