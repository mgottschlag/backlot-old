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

#ifndef _MAP_HPP_
#define _MAP_HPP_

#include "ReferenceCounted.hpp"
#include "Vector2.hpp"
#include "Rectangle.hpp"

#include <string>
#include <map>
#include <list>
#include <fstream>

namespace backlot
{
	static const unsigned int MAP_FORMAT_VERSION = 1;
	/**
	 * Map data. This class only can load precompiled maps. On the server and
	 * the client different parts of the file are loaded.
	 */
	class Map : public ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 */
			Map();
			/**
			 * Destructor.
			 */
			virtual ~Map();

			/**
			 * Loads a map file.
			 */
			virtual bool load(std::string name) = 0;

			/**
			 * Returns the map size.
			 */
			Vector2I getSize();
			/**
			 * Returns the path finding info. There are 4 bits used per square
			 * in the grid: The first says whether an entity can move from this
			 * position one unit to the right, the third to the left. The second
			 * and forth bit are for down/up respectively. Two positions are
			 * packed into one byte, with 0xF0 selecting the left of the two.
			 */
			unsigned char *getPathFindingInfo();

			/**
			 * Returns the height at the given position.
			 */
			float getHeight(Vector2F position);
			/**
			 * Returns the maximum height in the area.
			 */
			float getMaximumHeight(RectangleF area);
			/**
			 * Returns the minimum height in the area.
			 */
			float getMinimumHeight(RectangleF area);

			/**
			 * Returns true if the area along the line is accessible.
			 */
			bool isAccessible(Vector2F start, Vector2F end, float maxheight,
				Vector2F *collision = 0);

		protected:
			bool readHeader(std::ifstream &file);

			std::string name;
			Vector2I size;
			float *heightmap;
			unsigned char *accessible;
	};

	typedef SharedPointer<Map> MapPointer;
}

#endif
