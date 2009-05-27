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
#include "Buffer.hpp"
#include "NetworkData.hpp"

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
		this->mapname = mapname;
		// Load map
		map = Map::get(mapname);
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
		ENetAddress address;
		address.host = ENET_HOST_ANY;
		address.port = port;
		host = enet_host_create(&address, 32, 0, 0);
		if (host == 0)
		{
			std::cerr << "Could not create server socket." << std::endl;
			return false;
		}
		return true;
	}
	bool Server::destroy()
	{
		enet_host_destroy(host);
		map = 0;
		return false;
	}

	bool Server::update()
	{
		// Receive packets
		ENetEvent event;
		while (enet_host_service(host, &event, 0) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
				{
					std::cout << "Client connected." << std::endl;
					// Add to connected clients
					Client *newclient = new Client(event.peer);
					clients.push_back(newclient);
					newclient->setStatus(ECS_Connecting);
					// Send map name
					BufferPointer msg = new Buffer;
					msg->write8(EPT_InitialData);
					msg->writeString(mapname);
					newclient->send(msg);
					break;
				}
				case ENET_EVENT_TYPE_RECEIVE:
					enet_packet_destroy (event.packet);
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << "Client disconnected." << std::endl;
					break;
				default:
					break;
			}
		}
		// Game logic
		// Flush socket
		enet_host_flush(host);
		return true;
	}

	Server::Server()
	{
	}
}
