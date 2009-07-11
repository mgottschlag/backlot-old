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

#ifndef _QUADLIST_HPP_
#define _QUADLIST_HPP_

#include <vector>

#include "Rectangle.hpp"
using namespace backlot;

class TileSet;
class Quad;

class QuadList
{
	public:
		QuadList(TileSet *tileset, bool shadow);
		~QuadList();

		void addQuads(const std::vector<Quad> &quads, int x, int y);
		void addQuad(const Quad &quad, int x, int y);
		void addQuad(float *quadvertices, float *quadtexcoords);

		std::vector<QuadList*> split();

		int getVertexCount();
		float *getVertices();
		float *getTextureCoords();

		RectangleF getBoundingRect();

	private:
		TileSet *tileset;
		bool shadow;

		unsigned int vertexcount;
		float *vertices;
		float *texcoords;
		RectangleF boundingrect;
};

#endif
