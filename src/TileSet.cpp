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

#include "TileSet.hpp"
#include "Engine.hpp"
#include "Vector2.hpp"
#include "Rectangle.hpp"

#include "tinyxml.h"

#include <iostream>

namespace backlot
{
	TileSet::TileSet()
	{
		groundlayers = 0;
		shadowlayers = 0;
		highlayers = 0;
	}
	TileSet::~TileSet()
	{
	}

	bool TileSet::load(std::string name)
	{
		// Open texture
		TexturePointer texture = new Texture();
		if (!texture->load("tilesets/" + name + ".png"))
		{
			std::cerr << "Could not load tilesets/" << name << ".png" << std::endl;
			return false;
		}
		this->texture = texture;
		// Open XML file
		std::string filename = Engine::get().getGameDirectory() + "/tilesets/" + name + ".xml";
		TiXmlDocument xml(filename.c_str());
		if (!xml.LoadFile() || xml.Error())
		{
			std::cerr << "Could not load XML file " << name << ".xml: " << xml.ErrorDesc() << std::endl;
			return false;
		}
		TiXmlNode *node = xml.FirstChild("tileset");
		if (!node)
		{
			std::cerr << "Parser error: <tileset> not found." << std::endl;
			return false;
		}
		TiXmlElement *root = node->ToElement();
		// Load layer info
		TiXmlNode *layernode = root->FirstChild("layers");
		if (!layernode)
		{
			std::cerr << "No layer info given." << std::endl;
			return false;
		}
		TiXmlElement *layers = layernode->ToElement();
		if (!layers)
		{
			std::cerr << "No layer info given." << std::endl;
			return false;
		}
		groundlayers = 0;
		shadowlayers = 0;
		highlayers = 0;
		if (layers->Attribute("ground"))
			groundlayers = atoi(layers->Attribute("ground"));
		if (layers->Attribute("shadow"))
			shadowlayers = atoi(layers->Attribute("shadow"));
		if (layers->Attribute("high"))
			highlayers = atoi(layers->Attribute("high"));
		// Load tiles
		TiXmlNode *tilenode = root->FirstChild("tile");
		while (tilenode)
		{
			TiXmlElement *tile = tilenode->ToElement();
			if (!tile)
				continue;
			if (!loadTile(tile))
			{
				return false;
			}
			tilenode = node->IterateChildren("tile", tilenode);
		}
		return true;
	}

	void TileSet::getLayerCount(int &ground, int &shadow, int &high)
	{
		ground = groundlayers;
		shadow = shadowlayers;
		high = highlayers;
	}
	Tile *TileSet::getTile(std::string name)
	{
		std::map<std::string, Tile>::iterator it = tiles.find(name);
		if (it == tiles.end())
			return 0;
		return &it->second;
	}

	bool TileSet::loadTile(TiXmlElement *xml)
	{
		Tile tile(texture);
		// Tile attributes
		if (!xml->Attribute("name") || !xml->Attribute("size"))
		{
			std::cerr << "Missing tile name or size." << std::endl;
			return false;
		}
		tile.setSize(xml->Attribute("size"));
		if (xml->Attribute("accessible"))
		{
			tile.setAccessible(std::string("yes") == xml->Attribute("accessible"));
		}
		std::string tilename = xml->Attribute("name");
		// Load quads
		TiXmlNode *quadnode = xml->FirstChild("quad");
		while (quadnode)
		{
			TiXmlElement *quad = quadnode->ToElement();
			if (!quad)
				continue;
			// Quad attributes
			if (!quad->Attribute("layer") || !quad->Attribute("texture"))
			{
				std::cerr << "Missing quad layer or texture." << std::endl;
				return false;
			}
			int layer = atoi(quad->Attribute("layer"));
			RectangleI texturerect = quad->Attribute("texture");
			
			Vector2I offset;
			if (quad->Attribute("offset"))
			{
				offset = quad->Attribute("offset");
			}
			int rotated = 0;
			if (quad->Attribute("rotated"))
			{
				rotated = atoi(quad->Attribute("rotated"));
			}
			// Add quad to tile
			QuadInfo quadinfo;
			quadinfo.layer = layer;
			quadinfo.texturerect = texturerect;
			quadinfo.offset = offset;
			tile.addQuad(quadinfo);
			quadnode = xml->IterateChildren("quad", quadnode);
		}
		tiles.insert(std::pair<std::string, Tile>(tilename, tile));
		return true;
	}
}
