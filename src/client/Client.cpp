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
#include <cstring>

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

	bool Client::init(std::string address)
	{
		// Connect to server
		host = enet_host_create(NULL, 1, 0, 0);
		if (host == 0)
		{
			std::cerr << "Could not create client socket." << std::endl;
			return false;
		}
		std::string hostname = address.substr(0, address.find(":"));
		ENetAddress addr;
		enet_address_set_host(&addr, hostname.c_str());
		addr.port = atoi(address.substr(address.find(":") + 1).c_str());
		peer = enet_host_connect(host, &addr, 2);
		if (!peer)
		{
			enet_host_destroy(host);
			std::cerr << "Could not create client peer." << std::endl;
			return false;
		}
		ENetEvent event;
		if (enet_host_service(host, &event, 5000) > 0
			&& event.type == ENET_EVENT_TYPE_CONNECT)
		{
			std::cout << "Connected to " << address << std::endl;
		}
		else
		{
			enet_peer_reset(peer);
			enet_host_destroy(host);
			std::cerr << "Connection refused." << std::endl;
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
		// Disconnect properly
		ENetEvent event;
		enet_peer_disconnect(peer, 0);
		bool disconnected = false;
		while (enet_host_service(host, &event, 3000) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_RECEIVE:
					enet_packet_destroy(event.packet);
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << "Disconnected properly." << std::endl;
					disconnected = true;
					break;
				default:
					break;
			}
			if (disconnected)
				break;
		}
		// Destroy socket
		if (!disconnected)
			enet_peer_reset(peer);
		enet_host_destroy(host);
		map = 0;
		return false;
	}

	bool Client::update()
	{
		// Receive packets
		ENetEvent event;
		while (enet_host_service(host, &event, 0) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					std::cout << "Client connected." << std::endl;
					break;
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
		return true;
	}

	Client::Client()
	{
	}
}
