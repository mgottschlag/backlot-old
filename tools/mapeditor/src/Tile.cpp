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

#include "Tile.hpp"
#include "TileSet.hpp"

#include "tinyxml.h"

#include <iostream>

Tile::Tile(TileSet *tileset) : tileset(tileset)
{
}
Tile::~Tile()
{
}

bool Tile::load(TiXmlElement *xml)
{
	// Read attributes
	if (!xml->Attribute("name"))
	{
		std::cerr << "Tile: No name given." << std::endl;
		return false;
	}
	std::string name = xml->Attribute("name");
	if (!xml->Attribute("size") || !xml->Attribute("height"))
	{
		std::cerr << name << ": size or height missing." << std::endl;
		return false;
	}
	size = xml->Attribute("size");
	height = atof(xml->Attribute("height"));
	// Load quads
	TiXmlNode *quadnode = xml->FirstChild("quad");
	while (quadnode)
	{
		TiXmlElement *quad = quadnode->ToElement();
		if (!quad)
		{
			quadnode = xml->IterateChildren("quad", quadnode);
			continue;
		}
		// Quad attributes
		if (!quad->Attribute("height") || !quad->Attribute("texture"))
		{
			std::cerr << "Missing quad height or texture." << std::endl;
			return false;
		}
		float height = atoi(quad->Attribute("height"));
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
		bool shadow = false;
		if (quad->Attribute("type"))
		{
			shadow = !strcmp(quad->Attribute("type"), "shadow");
		}
		// Add quad to tile
		Quad quadinfo;
		quadinfo.height = height;
		quadinfo.texture = texturerect;
		quadinfo.offset = offset;
		quadinfo.rotated = rotated;
		if (shadow)
			shadowquads.push_back(quadinfo);
		else
			quads.push_back(quadinfo);
		quadnode = xml->IterateChildren("quad", quadnode);
	}
	std::cout << tileset->getName() << "." << name << " loaded." << std::endl;
	return false;
}

const Vector2F &Tile::getSize()
{
	return size;
}
float Tile::getHeight()
{
	return height;
}

void Tile::render(int x, int y)
{
}
void Tile::renderShadows(int x, int y)
{
}
