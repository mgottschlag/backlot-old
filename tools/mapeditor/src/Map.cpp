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
#include "Tile.hpp"
#include "TileSet.hpp"
#include "Game.hpp"
#include "QuadList.hpp"
#include "Rectangle.hpp"

#include <iostream>
#include <fstream>
#include <QFile>
#include <QDataStream>
#include <GL/gl.h>

Map &Map::get()
{
	static Map map;
	return map;
}

bool Map::create(std::string name)
{
	// Close old map
	if (isLoaded())
		close();
	// Create empty map
	this->name = name;
	width = 1;
	height = 1;
	tiles = new Tile*[width * height];
	tiles[0] = 0;
	return true;
}
bool Map::load(std::string name)
{
	// Close old map
	if (isLoaded())
		close();
	// Open file
	QFile file((Game::get().getPath() + "/maps/" + name + ".blm").c_str());
	if (!file.open(QIODevice::ReadOnly))
		return false;
	QDataStream in(&file);
	// Read header
	unsigned int version;
	in >> version;
	if (version != MAP_FORMAT_VERSION)
		return false;
	in >> width;
	in >> height;
	// Read list of used tiles
	unsigned int tilecount;
	in >> tilecount;
	std::vector<Tile*> usedtiles;
	for (unsigned int i = 0; i < tilecount; i++)
	{
		char *tilename;
		in >> tilename;
		if (tilename)
		{
			Tile *tile = TileSet::getTile(tilename);
			if (!tile)
				return false;
			usedtiles.push_back(TileSet::getTile(tilename));
		}
		else
			return false;
	}
	// Allocate tile info
	tiles = new Tile*[width * height];
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			tiles[y * width + x] = 0;
		}
	}
	// Read tiles
	unsigned int validtiles;
	in >> validtiles;
	for (unsigned int i = 0; i < validtiles; i++)
	{
		unsigned int x;
		unsigned int y;
		unsigned int tileindex;
		in >> x >> y >> tileindex;
		if (tileindex >= usedtiles.size())
			continue;
		if (x >= width || y >= height)
			continue;
		tiles[y * width + x] = usedtiles[tileindex];
	}
	// Read additional info
	// TODO
	// Read entities
	// TODO
	this->name = name;
	return true;
}
bool Map::close()
{
	if (tiles)
		delete[] tiles;
	name = "";
	return true;
}
bool Map::isLoaded()
{
	return name != "";
}
bool Map::save(std::string name)
{
	if (!isLoaded())
		return false;
	if (name == "")
		name = this->name;
	else
		this->name = name;
	// Open file
	QFile file((Game::get().getPath() + "/maps/" + name + ".blm").c_str());
	if (!file.open(QIODevice::WriteOnly))
		return false;
	QDataStream out(&file);
	// Write header
	out << MAP_FORMAT_VERSION;
	out << width;
	out << height;
	// Collect used tiles
	std::vector<Tile*> usedtiles;
	unsigned int validtiles = 0;
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			if (tiles[y * width + x])
			{
				validtiles++;
				Tile *tile = tiles[y * width + x];
				bool found = false;
				for (unsigned int i = 0; i < usedtiles.size(); i++)
				{
					if (usedtiles[i] == tile)
					{
						found = true;
						break;
					}
				}
				if (!found)
					usedtiles.push_back(tile);
			}
		}
	}
	// Write information about used tiles
	unsigned int tilecount = usedtiles.size();
	out << tilecount;
	for (unsigned int i = 0; i < usedtiles.size(); i++)
	{
		TileSet *tileset = usedtiles[i]->getTileSet();
		std::string name = tileset->getName() + "." + usedtiles[i]->getName();
		out << name.c_str();
	}
	// Write tiles
	out << validtiles;
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			if (tiles[y * width + x])
			{
				for (unsigned int i = 0; i < usedtiles.size(); i++)
				{
					if (usedtiles[i] == tiles[y * width + x])
					{
						out << x << y << i;
					}
				}
			}
		}
	}
	// Write additional info
	// TODO
	// Write entities
	// TODO
	return true;
}
std::string Map::getName()
{
	return name;
}

bool Map::compile(std::string name)
{
	if (!isLoaded())
		return false;
	if (name == "")
		name = this->name;
	// Open file
	std::ofstream file((Game::get().getPath() + "/maps/" + name + ".blc").c_str(),
		std::ofstream::out | std::ofstream::binary);
	if (!file)
		return false;
	// Write header
	unsigned int version = MAP_FORMAT_VERSION;
	file.write((const char*)&version, 4);
	file.write((const char*)&width, 4);
	file.write((const char*)&height, 4);
	// Write RLE height info
	unsigned short runlength = 1;
	float currentheight = 1000;
	if (tiles[0])
		currentheight = tiles[0]->getHeight();
	for (unsigned int i = 1; i < width * height; i++)
	{
		float currentheight2 = 1000;
		if (tiles[i])
			currentheight2 = tiles[i]->getHeight();
		if (currentheight2 != currentheight || runlength == 65535)
		{
			// Write chunk of data
			file.write((const char*)&runlength, 2);
			file.write((const char*)&currentheight, 4);
			runlength = 1;
			currentheight = currentheight2;
		}
		else
		{
			runlength++;
		}
	}
	if (runlength != 0)
	{
		// Write chunk of data
		file.write((const char*)&runlength, 2);
		file.write((const char*)&currentheight, 4);
	}
	// Write accessibility info
	// TODO
	// Write entities
	// TODO
	// Collect tile sets used
	std::vector<TileSet*> tilesets;
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			if (tiles[y * width + x])
			{
				TileSet *tileset = tiles[y * width + x]->getTileSet();
				bool found = false;
				for (unsigned int i = 0; i < tilesets.size(); i++)
				{
					if (tilesets[i] == tileset)
					{
						found = true;
						break;
					}
				}
				if (!found)
					tilesets.push_back(tileset);
			}
		}
	}
	// Create primary quad lists
	std::vector<QuadList*> primlists(tilesets.size() * 2, 0);
	for (unsigned int i = 0; i < tilesets.size(); i++)
	{
		primlists[i * 2] = new QuadList(tilesets[i], false);
		primlists[i * 2 + 1] = new QuadList(tilesets[i], true);
	}
	// Add tiles
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			Tile *tile = tiles[y * width + x];
			if (tile)
			{
				TileSet *tileset = tile->getTileSet();
				for (unsigned int i = 0; i < tilesets.size(); i++)
				{
					if (tilesets[i] == tileset)
					{
						primlists[i * 2]->addQuads(tile->getQuads(), x, y);
						primlists[i * 2 + 1]->addQuads(tile->getShadowQuads(), x, y);
						break;
					}
				}
			}
		}
	}
	RectangleF rect = primlists[0]->getBoundingRect();
	// Split lists
	std::vector<std::vector<QuadList*> > lists(tilesets.size() * 2, std::vector<QuadList*>());
	for (unsigned int i = 0; i < primlists.size(); i++)
	{
		lists[i] = primlists[i]->split();
	}
	// Write lists
	unsigned int size = lists.size();
	file.write((const char*)&size, 4);
	for (unsigned int i = 0; i < lists.size(); i++)
	{
		unsigned short length = primlists[i]->getTileSet()->getName().size();
		file.write((const char*)&length, 2);
		file.write(primlists[i]->getTileSet()->getName().c_str(), length);
		unsigned char shadow = primlists[i]->isShadow();
		file.write((const char*)&shadow, 1);
		size = lists[i].size();
		file.write((const char*)&size, 4);
		for (unsigned int j = 0; j < lists[i].size(); j++)
		{
			RectangleF rect = lists[i][j]->getBoundingRect();
			file.write((const char*)&rect.x, 4);
			file.write((const char*)&rect.y, 4);
			file.write((const char*)&rect.width, 4);
			file.write((const char*)&rect.height, 4);
			unsigned int vertexcount = lists[i][j]->getVertexCount();
			file.write((const char*)&vertexcount, 4);
			float *vertices = lists[i][j]->getVertices();
			float *texcoords = lists[i][j]->getTextureCoords();
			file.write((const char*)vertices, vertexcount * 12);
			file.write((const char*)texcoords, vertexcount * 8);
		}
	}
	// Clean up
	for (unsigned int i = 0; i < tilesets.size() * 2; i++)
	{
		delete primlists[i];
	}
	return true;
}

void Map::setWidth(unsigned int width)
{
	Tile **newtiles = new Tile*[width * height];
	unsigned int minwidth = width;
	if (this->width < width)
		minwidth = this->width;
	for (unsigned int y = 0; y < height; y++)
	{
		// Copy old tiles
		for (unsigned int x = 0; x < minwidth; x++)
		{
			newtiles[y * width + x] = tiles[y * this->width + x];
		}
		// Set new tiles to 0
		for (unsigned int x = this->width; x < width; x++)
		{
			newtiles[y * width + x] = 0;
		}
	}
	// Delete old tiles
	delete[] tiles;
	tiles = newtiles;
	this->width = width;
}
unsigned int Map::getWidth()
{
	return width;
}
void Map::setHeight(unsigned int height)
{
	Tile **newtiles = new Tile*[width * height];
	unsigned int minheight = height;
	if (this->height < height)
		minheight = this->height;
	for (unsigned int y = 0; y < minheight; y++)
	{
		// Copy old tiles
		for (unsigned int x = 0; x < width; x++)
		{
			newtiles[y * width + x] = tiles[y * width + x];
		}
	}
	for (unsigned int y = this->height; y < height; y++)
	{
		// Set new tiles to 0
		for (unsigned int x = 0; x < width; x++)
		{
			newtiles[y * width + x] = 0;
		}
	}
	// Delete old tiles
	delete[] tiles;
	tiles = newtiles;
	this->height = height;
}
unsigned int Map::getHeight()
{
	return height;
}

void Map::setTile(int x, int y, Tile *tile)
{
	if (x < 0 || y < 0
		|| x >= (int)width || y >= (int)height)
		return;
	tiles[y * width + x] = tile;
	// TODO: Remove overlapping tiles
}

Entity *Map::addEntity(std::string type, float x, float y)
{
	Entity *entity = new Entity(type, Vector2F(x, y));
	entities.push_back(entity);
	return entity;
}
Entity *Map::getEntity(float x, float y)
{
	std::list<Entity*>::iterator it = entities.begin();
	while (it != entities.end())
	{
		// Check entity rectangle
		RectangleF rect((*it)->getPosition().x - 0.5, (*it)->getPosition().y - 0.5,
			1, 1);
		if (rect.contains(Vector2F(x, y)))
			return *it;
		it++;
	}
	return 0;
}
void Map::removeEntity(Entity *entity)
{
	std::list<Entity*>::iterator it = entities.begin();
	while (it != entities.end())
	{
		// Remove entity
		if (*it == entity)
		{
			delete *it;
			entities.erase(it);
			return;
		}
		it++;
	}
}

void Map::render()
{
	// Render normal quads
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			if (tiles[y * width + x])
				tiles[y * width + x]->render(x, y);
		}
	}
	// Render shadows
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			if (tiles[y * width + x])
				tiles[y * width + x]->renderShadows(x, y);
		}
	}
}
void Map::renderEntities()
{
	std::list<Entity*>::iterator it = entities.begin();
	while (it != entities.end())
	{
		Vector2F pos = (*it)->getPosition();
		// Draw entity symbol
		glBegin(GL_QUADS);
			glTexCoord2d(0, 0);
			glVertex2f(pos.x - 0.5, pos.y - 0.5);
			glTexCoord2d(0, 1);
			glVertex2f(pos.x - 0.5, pos.y + 0.5);
			glTexCoord2d(1, 1);
			glVertex2f(pos.x + 0.5, pos.y + 0.5);
			glTexCoord2d(1, 0);
			glVertex2f(pos.x + 0.5, pos.y - 0.5);
		glEnd();
		it++;
	}
}

Map::Map()
{
	width = 0;
	height = 0;
	tiles = 0;
}
