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

namespace backlot
{
	Client::Client(ENetPeer *peer) : peer(peer)
	{
		status = ECS_Connecting;
		lastreceived = 0;
		lag = 0;
	}
	Client::~Client()
	{
	}

	void Client::setStatus(ClientStatus status)
	{
		this->status = status;
	}
	ClientStatus Client::getStatus()
	{
		return status;
	}

	void Client::send(BufferPointer buffer, bool reliable)
	{
		ENetPacket *packet = enet_packet_create(buffer->getData(),
			buffer->getSize(), reliable?ENET_PACKET_FLAG_RELIABLE:0);
		enet_peer_send(peer, 0, packet);
	}

	void Client::setAcknowledgedPacket(int time)
	{
		lastreceived = time;
	}
	int Client::getAcknowledgedPacket()
	{
		return lastreceived;
	}
	void Client::setLag(int lag)
	{
		this->lag = lag;
	}
	int Client::getLag()
	{
		return lag;
	}
	void Client::setEntityActive(int entity, bool active)
	{
		this->active[entity] = active;
	}
	bool Client::isEntityActive(int entity)
	{
		return active[entity];
	}
	void Client::setID(int id)
	{
		this->id = id;
	}
	int Client::getID()
	{
		return id;
	}
}
