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
#include "Engine.hpp"
#ifdef SERVER
#include "Game.hpp"
#endif

#include <iostream>
#include <fstream>

namespace backlot
{
	Map::Map() : ReferenceCounted()
	{
		heightmap = 0;
	}
	Map::~Map()
	{
		// Destroy map
		if (heightmap)
			delete[] heightmap;
		if (accessible)
			delete[] accessible;
		// TODO
	}

	Vector2I Map::getSize()
	{
		return size;
	}
	unsigned char *Map::getPathFindingInfo()
	{
		return accessible;
	}

	float Map::getHeight(Vector2F position)
	{
		Vector2I ipos = position;
		if (ipos.x < 0 || ipos.y < 0)
			return 1000;
		if (ipos.x >= size.x || ipos.y >= size.y)
			return 1000;
		return heightmap[ipos.x + ipos.y * size.x];
	}
	float Map::getMaximumHeight(RectangleF area)
	{
		// Align area to integer boundaries
		Vector2I min(floor(area.x), floor(area.y));
		Vector2I max(ceil(area.x + area.width), ceil(area.y + area.height));
		// Loop through all squares
		float maxheight = -1000;
		for (int y = min.y; y < max.y; y++)
		{
			for (int x = min.x; x < max.x; x++)
			{
				float height = getHeight(Vector2F(x, y));
				if (height > maxheight)
					maxheight = height;
			}
		}
		return maxheight;
	}
	float Map::getMinimumHeight(RectangleF area)
	{
		// Align area to integer boundaries
		Vector2I min(floor(area.x), floor(area.y));
		Vector2I max(ceil(area.x + area.width), ceil(area.y + area.height));
		// Loop through all squares
		float minheight = 1000;
		for (int y = min.y; y < max.y; y++)
		{
			for (int x = min.x; x < max.x; x++)
			{
				float height = getHeight(Vector2F(x, y));
				if (height < minheight)
					minheight = height;
			}
		}
		return minheight;
	}

	bool Map::isAccessible(Vector2F start, Vector2F end, float maxheight,
		Vector2F *collision)
	{
		// Special cases
		if (start.x == end.x)
		{
			// TODO
			return true;
		}
		if (start.y == end.y)
		{
			// TODO
			return true;
		}
		// Go through the grid along the vector
		int startx = (int)start.x;
		int starty = (int)start.y;
		int dirx = (end.x > start.x) ? 1 : -1;
		int diry = (end.y > start.y) ? 1 : -1;
		int nextx = (int)((dirx == 1) ? floor(start.x + 1.001) : ceil(start.x - 1.001));
		int nexty = (int)((diry == 1) ? floor(start.y + 1.001) : ceil(start.y - 1.001));
		float nextx_y = start.y + (start.x - nextx) / (end.x - start.x) * (end.y - start.y);
		float nexty_x = start.x + (start.y - nexty) / (end.y - start.y) * (end.x - start.x);
		float tmaxxsquared = (start.x - nextx) * (start.x - nextx) + (start.y - nextx_y) * (start.y - nextx_y);
		float tmaxysquared = (start.y - nexty) * (start.y - nexty) + (start.x - nexty_x) * (start.x - nexty_x);
		float lengthsquared = (end - start).getLengthSquared();
		if (tmaxxsquared >= lengthsquared && tmaxysquared >= lengthsquared)
		{
			// End reached
			return true;
		}
		if (tmaxxsquared < tmaxysquared)
		{
			int nextx = startx + dirx;
			float nextheight = getHeight(Vector2F(nextx, starty));
			if (nextheight > maxheight)
			{
				// Collision
				// TODO: Collision info
				return false;
			}
			else
			{
				// Continue search
				// TODO: Can be optimized.
				return isAccessible(Vector2F((float)nextx, nextx_y), end, maxheight, collision);
			}
		}
		else
		{
			int nexty = starty + diry;
			float nextheight = getHeight(Vector2F(startx, nexty));
			if (nextheight > maxheight)
			{
				// Collision
				// TODO: Collision info
				return false;
			}
			else
			{
				// Continue search
				// TODO: Can be optimized.
				return isAccessible(Vector2F(nexty_x, (float)nexty), end, maxheight, collision);
			}
		}
	}

	bool Map::readHeader(std::ifstream &file)
	{
		// Read header
		unsigned int version = 0;
		file.read((char*)&version, 4);
		if (version != MAP_FORMAT_VERSION)
		{
			std::cerr << "Wrong map version (" << version << " vs " << MAP_FORMAT_VERSION << ")." << std::endl;
			return false;
		}
		file.read((char*)&size.x, 4);
		file.read((char*)&size.y, 4);
		// Read height info
		heightmap = new float[size.x * size.y];
		int read = 0;
		while (read < size.x * size.y)
		{
			unsigned short runlength = 0;
			file.read((char*)&runlength, 2);
			if (!runlength)
			{
				std::cerr << "Error while reading height data." << std::endl;
				delete[] heightmap;
				heightmap = 0;
				return false;
			}
			float height = 0;
			file.read((char*)&height, 4);
			for (unsigned int i = 0; i < runlength; i++)
				heightmap[read + i] = height;
			read += runlength;
		}
		// Read accessibility info
		accessible = new unsigned char[(size.x * size.y + 1) / 2];
		file.read((char*)accessible, (size.x * size.y + 1) / 2);
		return true;
	}
}
