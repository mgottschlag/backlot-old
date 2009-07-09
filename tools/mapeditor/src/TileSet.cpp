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

#include <iostream>

TileSet::~TileSet()
{
}

TileSet *TileSet::get(std::string name)
{
}
Tile *TileSet::getTile(std::string name)
{
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
}
std::vector<std::string> TileSet::getTiles()
{
}

TileSet::TileSet()
{
}

bool TileSet::load(std::string name)
{
	return true;
}

std::map<std::string, TileSet*> TileSet::tilesets;
