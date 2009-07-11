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

#ifndef _TILE_HPP_
#define _TILE_HPP_

#include "Vector2.hpp"
#include "Rectangle.hpp"
using namespace backlot;

#include <vector>

class TileSet;
class TiXmlElement;

struct Quad
{
	float height;
	int rotated;
	Vector2I offset;
	RectangleI texture;
};

class Tile
{
	public:
		Tile(TileSet *tileset);
		~Tile();

		bool load(TiXmlElement *xml);

		const Vector2F &getSize();
		float getHeight();

		void render(int x, int y);
		void renderShadows(int x, int y);
	private:
		void drawQuad(Quad &quad);

		TileSet *tileset;

		Vector2F size;
		float height;
		std::vector<Quad> quads;
		std::vector<Quad> shadowquads;
};

#endif
