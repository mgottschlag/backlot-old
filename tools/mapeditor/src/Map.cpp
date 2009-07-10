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
	// Load map
	// TODO
	return false;
}
bool Map::close()
{
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
	return false;
}
std::string Map::getName()
{
	return name;
}

bool Map::compile(std::string name)
{
	return false;
}

void Map::setWidth(int width)
{
	Tile **newtiles = new Tile*[width * height];
	int minwidth = width;
	if (this->width < width)
		minwidth = this->width;
	for (int y = 0; y < height; y++)
	{
		// Copy old tiles
		for (int x = 0; x < minwidth; x++)
		{
			newtiles[y * width + x] = tiles[y * this->width + x];
		}
		// Set new tiles to 0
		for (int x = this->width; x < width; x++)
		{
			newtiles[y * width + x] = 0;
		}
	}
	// Delete old tiles
	delete[] tiles;
	tiles = newtiles;
	this->width = width;
}
int Map::getWidth()
{
	return width;
}
void Map::setHeight(int height)
{
	Tile **newtiles = new Tile*[width * height];
	int minheight = height;
	if (this->height < height)
		minheight = this->height;
	for (int y = 0; y < minheight; y++)
	{
		// Copy old tiles
		for (int x = 0; x < width; x++)
		{
			newtiles[y * width + x] = tiles[y * width + x];
		}
	}
	for (int y = this->height; y < height; y++)
	{
		// Set new tiles to 0
		for (int x = 0; x < width; x++)
		{
			newtiles[y * width + x] = 0;
		}
	}
	// Delete old tiles
	delete[] tiles;
	tiles = newtiles;
	this->height = height;
}
int Map::getHeight()
{
	return height;
}

void Map::render()
{
}

Map::Map()
{
	width = 0;
	height = 0;
	tiles = 0;
}
