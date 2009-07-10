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

#ifndef _TILESET_HPP_
#define _TILESET_HPP_

#include "Vector2.hpp"
using namespace backlot;

#include <string>
#include <vector>
#include <map>

class Tile;

class TileSet
{
	public:
		~TileSet();

		static TileSet *get(std::string name);
		static Tile *getTile(std::string name);
		static void loadAll();
		static std::vector<std::string> getTileSets();
		static std::vector<std::string> getTiles();
		std::map<std::string, Tile*> &getTileInfo();

		static void loadTextures();
		static void loadPreviewTextures();
		unsigned int getTexture();
		unsigned int getPreviewTexture();
		Vector2I getTextureSize();

		std::string getName();

	private:
		TileSet();

		bool load(std::string name);

		static unsigned int loadTexture(std::string name, Vector2I *size = 0);

		std::string name;
		std::map<std::string, Tile*> tiles;
		unsigned int texture;
		unsigned int prevtexture;
		Vector2I texturesize;

		static std::map<std::string, TileSet*> tilesets;
};

#endif
