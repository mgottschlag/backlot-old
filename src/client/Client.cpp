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

#include "Client.hpp"
#include "Map.hpp"

#include <iostream>

namespace backlot
{
	Client &Client::get()
	{
		static Client client;
		return client;
	}
	Client::~Client()
	{
	}

	bool Client::init()
	{
		// Connect to server
		host = enet_host_create(NULL, 1, 0, 0);
		if (host == 0)
		{
			std::cerr << "Could not create client socket." << std::endl;
			return false;
		}
		// Load map
		map = Map::get("test");
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
		map->setVisible(true);
		return true;
	}
	bool Client::destroy()
	{
		enet_host_destroy(host);
		map = 0;
		return false;
	}

	bool Client::update()
	{
		return true;
	}

	Client::Client()
	{
	}
}
