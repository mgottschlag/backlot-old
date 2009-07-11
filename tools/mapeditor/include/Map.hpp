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

#include <QObject>

class Tile;

static const unsigned int MAP_FORMAT_VERSION = 1;

class Map : public QObject
{
	Q_OBJECT

	public:
		static Map &get();

		bool create(std::string name);
		bool load(std::string name);
		bool close();
		bool isLoaded();
		bool save(std::string name = "");
		std::string getName();

		bool compile(std::string name = "");

		void setWidth(int width);
		int getWidth();
		void setHeight(int height);
		int getHeight();

		void setTile(int x, int y, Tile *tile);

		void render();
	private:
		Map();

		std::string name;
		int width;
		int height;
		Tile **tiles;
};

#endif
