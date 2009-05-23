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

#include "Server.hpp"
#include "Map.hpp"

#include <iostream>

namespace backlot
{
	Server &Server::get()
	{
		static Server server;
		return server;
	}
	Server::~Server()
	{
	}

	bool Server::init(int port, std::string mapname, int maxclients)
	{
		// Load map
		MapPointer map = Map::get(mapname);
		if (map.isNull())
		{
			std::cerr << "Could not load map." << std::endl;
			return false;
		}
		if (!map->isCompiled())
		{
			if (!map->compile())
			{
				std::cerr << "Could not compile map." << std::endl;
				return false;
			}
		}
		std::cout << "Map is ready." << std::endl;
		// Create network socket
		return true;
	}
	bool Server::destroy()
	{
		return false;
	}

	bool Server::update()
	{
		return true;
	}

	Server::Server()
	{
	}
}
