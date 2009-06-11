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
#include "Engine.hpp"
#include "NetworkData.hpp"
#include "Buffer.hpp"
#include "Bullet.hpp"
#include "Effect.hpp"

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
		map = Map::get(mapname);
		if (map.isNull())
		{
			enet_peer_reset(peer);
			enet_host_destroy(host);
			std::cerr << "Could not load map." << std::endl;
			return false;
		}
		if (!map->isCompiled())
		{
			if (!map->compile())
			{
				enet_peer_reset(peer);
				enet_host_destroy(host);
				std::cerr << "Could not compile map." << std::endl;
				return false;
			}
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
		Bullet::clearBullets();
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
				{
					// Copy packet
					BufferPointer msg = new Buffer(event.packet->data,
						event.packet->dataLength, true);
					enet_packet_destroy(event.packet);
					PacketType type = (PacketType)msg->read8();
					// Parse packet
					if (type == EPT_NewPlayer)
					{
						// Create player
						int id = msg->read32();
						int hitpoints = msg->read8();
						bool local = msg->read8();
						PlayerPointer newplayer = new Player();
						newplayer->setID(id);
						newplayer->setHitpoints(hitpoints);
						newplayer->setLocal(local);
						newplayer->load();
						newplayer->setVisible(true);
						players.push_back(newplayer);
					}
					else if (type == EPT_Update)
					{
						int count = msg->read16();
						// Loop through sent players
						for (int i = 0; i < count; i++)
						{
							int id = msg->read32();
							// Get player
							PlayerPointer player = 0;
							for (unsigned int j = 0; j < players.size(); j++)
							{
								if (players[j]->getID() == id)
								{
									player = players[j];
									break;
								}
							}
							// Update data
							uint8_t keys = msg->read8();
							float rotation = msg->readFloat();
							float x = msg->readFloat();
							float y = msg->readFloat();
							if (player.isNull())
								continue;
							player->setPosition(Vector2F(x, y));
							player->setRotation(rotation);
							player->receiveKeys(keys);
						}
					}
					else if (type == EPT_NewWeapon)
					{
						int playerid = msg->read32();
						int id = msg->read16();
						int weaponid = msg->read32();
						std::string name = msg->readString();
						// Load weapon
						WeaponPointer weapon = Weapon::get("plasma", weaponid);
						if (weapon.isNull())
						{
							std::cerr << "Could not load weapon" << name << "." << std::endl;
							continue;
						}
						// Get player
						PlayerPointer player = 0;
						for (unsigned int j = 0; j < players.size(); j++)
						{
							if (players[j]->getID() == playerid)
							{
								player = players[j];
								break;
							}
						}
						if (player.isNull())
							continue;
						// Add weapon to player
						player->addWeapon(id, weapon);
					}
					else if (type == EPT_UpdateWeapon)
					{
						WeaponState *state = Player::getLocalPlayer()->getCurrentWeapon();
						state->currentmagazine = msg->read8();
					std::cout << "magazine: " << state->currentmagazine << std::endl;
						state->reserve = msg->read8();
					}
					else if (type == EPT_Projectile)
					{
						// Read bullet information
						Vector2F position;
						position.x = msg->readFloat();
						position.y = msg->readFloat();
						Vector2F speed;
						speed.x = msg->readFloat();
						speed.y = msg->readFloat();
						int weaponid = msg->read32();
						// Create bullet
						WeaponPointer weapon = Weapon::get(weaponid);
						if (weapon)
						{
							BulletPointer bullet = new Bullet(weapon);
							bullet->setPosition(position);
							bullet->setSpeed(speed);
							weapon->playSound();
						}
						else
						{
							std::cout << "No weapon for bullet." << std::endl;
						}
					}
					else if (type == EPT_Explosion)
					{
						// Read explosion info
						int weaponid = msg->read32();
						Vector2F position;
						position.x = msg->readFloat();
						position.y = msg->readFloat();
						// Create explosion
						WeaponPointer weapon = Weapon::get(weaponid);
						float radius;
						int damage;
						TexturePointer texture;
						if (weapon && weapon->getExplosion(radius, damage, texture))
						{
							EffectPointer explosion = new Effect();
							if (explosion->load(texture, Vector2I(4, 4), 0))
							{
								explosion->setPeriod(300);
								explosion->setPosition(position);
								explosion->play(1);
							}
						}
						else
						{
							std::cout << "No weapon for explosion." << std::endl;
						}
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
		// Game logic
		Bullet::updateAll();
		return true;
	}

	Client::Client()
	{
	}

	void Client::send(BufferPointer buffer, bool reliable)
	{
		ENetPacket *packet = enet_packet_create(buffer->getData(),
			buffer->getSize(), reliable?ENET_PACKET_FLAG_RELIABLE:0);
		enet_peer_send(peer, 0, packet);
	}

	MapPointer Client::getMap()
	{
		return map;
	}
}
