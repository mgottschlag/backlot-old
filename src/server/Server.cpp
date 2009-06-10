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

	void Server::sendToAll(BufferPointer buffer, bool reliable)
	{
		for (unsigned int i = 0; i < clients.size(); i++)
		{
			clients[i]->send(buffer, reliable);
		}
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
					event.peer->data = newclient;
					// Send map name
					BufferPointer msg = new Buffer;
					msg->write8(EPT_InitialData);
					msg->writeString(mapname);
					newclient->send(msg);
					break;
				}
				case ENET_EVENT_TYPE_RECEIVE:
				{
					// Copy packet
					BufferPointer msg = new Buffer(event.packet->data,
						event.packet->dataLength, true);
					enet_packet_destroy(event.packet);
					PacketType type = (PacketType)msg->read8();
					Client *client = (Client*)event.peer->data;
					// Parse packet
					if (type == EPT_Ready)
					{
						// Send other players to the client
						for (unsigned int i = 0; i < players.size(); i++)
						{
							BufferPointer msg = new Buffer();
							msg->write8(EPT_NewPlayer);
							msg->write32(players[i]->getID());
							msg->write8(0);
							client->send(msg, true);
							// Also send weapons
							std::map<int, WeaponState> &weapons = players[i]->getWeapons();
							std::map<int, WeaponState>::iterator it = weapons.begin();
							while (it != weapons.end())
							{
								BufferPointer msg = new Buffer();
								msg->write8(EPT_NewWeapon);
								msg->write32(players[i]->getID());
								msg->write16(it->first);
								msg->write32(it->second.weapon->getID());
								msg->writeString("plasma");
								client->send(msg, true);
								it++;
							}
						}
						// Create player
						PlayerPointer newplayer = new Player();
						newplayer->setOwner(client);
						newplayer->load();
						players.push_back(newplayer);
						// Send message about the player to other clients
						BufferPointer msg = new Buffer();
						msg->write8(EPT_NewPlayer);
						msg->write32(newplayer->getID());
						msg->write8(newplayer->getHitpoints());
						msg->write8(0);
						for (unsigned int i = 0; i < clients.size(); i++)
						{
							if (clients[i] != client)
							{
								clients[i]->send(msg, true);
							}
						}
						msg->setPosition(6);
						msg->write8(1);
						client->send(msg, true);
						// Initial weapon
						WeaponPointer weapon = Weapon::get("plasma");
						if (weapon)
						{
							int id = newplayer->addWeapon(weapon);
							BufferPointer msg = new Buffer();
							msg->write8(EPT_NewWeapon);
							msg->write32(newplayer->getID());
							msg->write16(id);
							msg->write32(weapon->getID());
							msg->writeString("plasma");
							for (unsigned int i = 0; i < clients.size(); i++)
							{
								clients[i]->send(msg, true);
							}
						}
					}
					else if (type == EPT_Keys)
					{
						int id = msg->read32();
						uint8_t keys = msg->read8();
						// Get player
						PlayerPointer player = 0;
						for (unsigned int i = 0; i < players.size(); i++)
						{
							if (players[i]->getID() == id)
							{
								player = players[i];
								break;
							}
						}
						if (!player || player->getOwner() != event.peer->data)
						{
							enet_peer_disconnect(event.peer, 0);
							break;
						}
						// Set keyboard info
						player->setKeys(keys);
					}
					else if (type == EPT_Rotation)
					{
						int id = msg->read32();
						float rotation = msg->readFloat();
						// Get player
						PlayerPointer player = 0;
						for (unsigned int i = 0; i < players.size(); i++)
						{
							if (players[i]->getID() == id)
							{
								player = players[i];
								break;
							}
						}
						if (!player || player->getOwner() != event.peer->data)
						{
							enet_peer_disconnect(event.peer, 0);
							break;
						}
						// Set keyboard info
						player->setRotation(rotation);
					}
					else
					{
						// Invalid packet, disconnect client
						enet_peer_disconnect(event.peer, 0);
					}
					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << "Client disconnected." << std::endl;
					// Remove player
					for (unsigned int i = 0; i < players.size(); i++)
					{
						if (players[i]->getOwner() == event.peer->data)
						{
							players.erase(players.begin() + i);
							break;
						}
					}
					// Remove client from client list
					for (unsigned int i = 0; i < clients.size(); i++)
					{
						if (clients[i]->getPeer() == event.peer)
						{
							delete clients[i];
							clients.erase(clients.begin() + i);
							break;
						}
					}
					break;
				default:
					break;
			}
		}
		// Game logic
		for (unsigned int i = 0; i < players.size(); i++)
			players[i]->think();
		// Send updates to clients
		for (unsigned int i = 0; i < clients.size(); i++)
		{
			Client *client = clients[i];
			BufferPointer msg = new Buffer();
			msg->write8(EPT_Update);
			msg->write16(players.size());
			for (unsigned int j = 0; j < players.size(); j++)
			{
				msg->write32(players[j]->getID());
				msg->write8(players[j]->getKeys());
				msg->writeFloat(players[j]->getRotation());
				Vector2F position = players[j]->getPosition();
				msg->writeFloat(position.x);
				msg->writeFloat(position.y);
			}
			client->send(msg);
		}
		// Flush socket
		enet_host_flush(host);
		return true;
	}

	Server::Server()
	{
	}
}
