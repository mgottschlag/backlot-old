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
#include "Game.hpp"
#include "Tile.hpp"

#include "tinyxml.h"

#include <iostream>
#include <QImage>
#include <QGLWidget>

TileSet::~TileSet()
{
}

TileSet *TileSet::get(std::string name)
{
	std::map<std::string, TileSet*>::iterator it = tilesets.find(name);
	if (it == tilesets.end())
		return 0;
	return it->second;
}
Tile *TileSet::getTile(std::string name)
{
	// Split name
	std::string tilesetname = name.substr(0, name.find("."));
	std::string tilename = name.substr(name.find(".") + 1);
	// Get tile set
	TileSet *tileset = get(tilesetname);
	if (!tileset)
		return 0;
	// Get tile
	std::map<std::string, Tile*> &tiles = tileset->getTileInfo();
	std::map<std::string, Tile*>::iterator it = tiles.find(tilename);
	if (it == tiles.end())
		return 0;
	return it->second;
}
void TileSet::loadAll()
{
	std::vector<std::string> tilesetnames = Game::get().getTileSets();
	for (unsigned int i = 0; i < tilesetnames.size(); i++)
	{
		std::cout << "Loading tile set \"" << tilesetnames[i] << "\"." << std::endl;
		// Load tile set
		TileSet *tileset = new TileSet();
		if (!tileset->load(tilesetnames[i]))
		{
			std::cout << "Could not load tile set!" << std::endl;
			delete tileset;
			continue;
		}
		// Add to tile set list
		tilesets.insert(std::pair<std::string, TileSet*>(tilesetnames[i], tileset));
	}
}
std::vector<std::string> TileSet::getTileSets()
{
	std::vector<std::string> tilesetnames;
	std::map<std::string, TileSet*>::iterator it = tilesets.begin();
	while (it != tilesets.end())
	{
		tilesetnames.push_back(it->first);
		it++;
	}
	return tilesetnames;
}
std::vector<std::string> TileSet::getTiles()
{
	std::vector<std::string> tilenames;
	// Loop through all tile sets
	std::map<std::string, TileSet*>::iterator it = tilesets.begin();
	while (it != tilesets.end())
	{
		// Loop through all tileset
		std::map<std::string, Tile*> &tiles = it->second->getTileInfo();
		std::map<std::string, Tile*>::iterator it2 = tiles.begin();
		while (it2 != tiles.end())
		{
			// Add the tile name to the list
			tilenames.push_back(it->first + "." + it2->first);
			it2++;
		}
		it++;
	}
	return tilenames;
}
std::map<std::string, Tile*> &TileSet::getTileInfo()
{
	return tiles;
}

void TileSet::loadTextures()
{
	std::vector<std::string> tilenames;
	// Loop through all tile sets
	std::map<std::string, TileSet*>::iterator it = tilesets.begin();
	while (it != tilesets.end())
	{
		it->second->prevtexture = loadTexture(it->first, &it->second->texturesize);
		it++;
	}
}
void TileSet::loadPreviewTextures()
{
	std::vector<std::string> tilenames;
	// Loop through all tile sets
	std::map<std::string, TileSet*>::iterator it = tilesets.begin();
	while (it != tilesets.end())
	{
		it->second->texture = loadTexture(it->first);
		it++;
	}
}
unsigned int TileSet::getTexture()
{
	return texture;
}
unsigned int TileSet::getPreviewTexture()
{
	return prevtexture;
}
Vector2I TileSet::getTextureSize()
{
	return texturesize;
}

std::string TileSet::getName()
{
	return name;
}

TileSet::TileSet()
{
}

bool TileSet::load(std::string name)
{
	std::string filename = Game::get().getPath() + "/tilesets/" + name + ".xml";
	this->name = name;
	// Open XML file
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
	// Load tiles
	TiXmlNode *tilenode = root->FirstChild("tile");
	while (tilenode)
	{
		TiXmlElement *tile = tilenode->ToElement();
		if (tile)
		{
			Tile *newtile = new Tile(this);
			if (newtile->load(tile))
			{
				tiles.insert(std::pair<std::string, Tile*>(tile->Attribute("name"), newtile));
			}
		}
		tilenode = node->IterateChildren("tile", tilenode);
	}
	return true;
}

unsigned int TileSet::loadTexture(std::string name, Vector2I *size)
{
	// Load the image
	std::string filename = Game::get().getPath() + "/tilesets/" + name + ".png";
	QImage orig;
	orig.load(filename.c_str());
	QImage converted = QGLWidget::convertToGLFormat(orig);
	// Return the size
	if (size)
	{
		size->x = orig.width();
		size->y = orig.height();
	}
	// Create a texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, converted.width(), converted.height(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, converted.bits());
	return texture;
}

std::map<std::string, TileSet*> TileSet::tilesets;
