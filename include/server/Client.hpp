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

#ifndef _CLIENT_HPP_
#define _CLIENT_HPP_

#include "Buffer.hpp"

#include <enet/enet.h>

namespace backlot
{
	enum ClientStatus
	{
		ECS_Connecting,
		ECS_Spectator,
		ECS_Playing
	};

	/**
	 * Server-side client information. Here both network connection information
	 * and the time of the last packet which definately has been received by the
	 * player are tracked.
	 * The latter provides a safe way to send only the info which really is
	 * needed.
	 */
	class Client
	{
		public:
			/**
			 * Constructor.
			 * @param peer Network connection connected to this client.
			 */
			Client(ENetPeer *peer);
			/**
			 * Destructor.
			 */
			~Client();

			/**
			 * Sets the status of this client. The status sets whether this
			 * client will get position updates (spectator or playing) or not
			 * (connecting).
			 * @param status New client status.
			 */
			void setStatus(ClientStatus status);
			/**
			 * Returns the client status.
			 */
			ClientStatus getStatus();

			/**
			 * Returns the network connection connected to this client.
			 */
			ENetPeer *getPeer()
			{
				return peer;
			}
			/**
			 * Sends a packet to the client.
			 * @param buffer Data to send.
			 * @param reliable If set to true, the data will be sent reliably.
			 */
			void send(BufferPointer buffer, bool reliable = false);

			/**
			 * Sets the time of the last packet which the client definately got.
			 */
			void setAcknowledgedPacket(int time);
			/**
			 * Returns the time of the last received packet.
			 */
			int getAcknowledgedPacket();
			/**
			 * Tracks whether entity updates for this entity are sent to
			 * this client.
			 */
			void setEntityActive(int entity, bool active);
			/**
			 * Returns whether entity updates for this entity are sent to the
			 * client.
			 */
			bool isEntityActive(int entity);
		private:
			ENetPeer *peer;
			ClientStatus status;
			int lastreceived;
			bool active[65535];
	};
}

#endif
