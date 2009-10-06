/*
Copyright (C) 2009  Mathias Gottschlag, Simon Kerler

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
#include "Engine.hpp"
#include "NetworkData.hpp"
#include "Buffer.hpp"
#include "Effect.hpp"
#include "Game.hpp"

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
		// Receive initial data
		uint64_t starttime = Engine::get().getTime();
		std::string mapname;
		bool gotdata = 0;
		while (enet_host_service(host, &event, 1000) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_RECEIVE:
				{
					// Copy packet
					BufferPointer msg = new Buffer(event.packet->data,
						event.packet->dataLength, true);
					enet_packet_destroy(event.packet);
					PacketType type = (PacketType)msg->read8();
					// Discard everything but the data we need
					if (type == EPT_InitialData)
					{
						mapname = msg->readString();
						int clientid = msg->read16();
						Game::get().load(mapname, clientid);
						gotdata = true;
					}
					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << "Server disconnected." << std::endl;
					enet_peer_reset(peer);
					enet_host_destroy(host);
					return false;
				default:
					break;
			}
			if (gotdata)
				break;
			if (Engine::get().getTime() - starttime > 10000000)
				break;
		}
		if (!gotdata)
		{
			enet_peer_reset(peer);
			enet_host_destroy(host);
			std::cerr << "Did not receive server info." << std::endl;
			return false;
		}
		// Load map
		map = ClientMap::get(mapname);
		if (map.isNull())
		{
			enet_peer_reset(peer);
			enet_host_destroy(host);
			std::cerr << "Could not load map." << std::endl;
			return false;
		}
		std::cout << "Map is ready." << std::endl;
		map->setVisible(true);
		// Send message back to the server
		BufferPointer msg = new Buffer();
		msg->write8(EPT_Ready);
		send(msg, true);
		return true;
	}
	bool Client::destroy()
	{
		if (!map)
			return true;
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
		// Remove entities
		Game::get().destroy();
		// Destroy socket
		if (!disconnected)
			enet_peer_reset(peer);
		enet_host_destroy(host);
		map = 0;
		return true;
	}

	void Client::setAcknowledgedPacket(int time)
	{
		lastpacket = time;
		Game::get().setAcknowledgedPacket(time);
	}
	int Client::getAcknowledgedPacket()
	{
		return lastpacket;
	}

	bool Client::update()
	{
		if (!map)
			return true;
		uint64_t start = Engine::getTime();
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
				{
					// Copy packet
					BufferPointer msg = new Buffer(event.packet->data,
						event.packet->dataLength, true);
					enet_packet_destroy(event.packet);
					PacketType type = (PacketType)msg->read8();
					// Parse packet
					if (type == EPT_EntityCreated)
					{
						std::cout << "New entity." << std::endl;
						int id = msg->read16();
						int owner = msg->read16();
						std::string type = msg->readString();
						// Create entity
						EntityPointer entity = Game::get().addEntity(type, owner,
							id, msg);
						if (!entity)
						{
							std::cerr << "Could not create entity." << std::endl;
							return false;
						}
						std::cout << "Created client entity." << std::endl;
					}
					else if (type == EPT_EntityDeleted)
					{
						std::cout << "Entity deleted." << std::endl;
						int id = msg->read16();
						Game::get().removeEntity(id);
					}
					else if (type == EPT_Update)
					{
						Game::get().injectUpdates(msg);
					}
					else if (type == EPT_UpdateReceived)
					{
						// Get time info from the server
						unsigned int time = msg->read32();
						std::cout << time << " acked." << std::endl;
						//unsigned int rtt = msg->read16();
						setAcknowledgedPacket(time);
						Game::get().setLag(Game::get().getTime() - time);
					}
					else
					{
						std::cerr << "Unknown packet received." << std::endl;
					}
					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << "Client disconnected." << std::endl;
					break;
				default:
					break;
			}
		}
		uint64_t end = Engine::getTime();
		float updatetime = (float)(end - start) / 20000;
		std::cout << "Client:" << updatetime << std::endl;
		// Game logic
		Game::get().update();
		return true;
	}

	Client::Client()
	{
		lastpacket = 0;
	}

	void Client::send(BufferPointer buffer, bool reliable)
	{
		ENetPacket *packet = enet_packet_create(buffer->getData(),
			buffer->getSize(), reliable?ENET_PACKET_FLAG_RELIABLE:0);
		enet_peer_send(peer, 0, packet);
	}

	MapPointer Client::getMap()
	{
		return map.get();
	}
}
