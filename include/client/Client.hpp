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

#include "Map.hpp"
#include "Buffer.hpp"

#include <enet/enet.h>
#include <vector>

namespace backlot
{
	class Client
	{
		public:
			static Client &get();
			~Client();

			bool init(std::string address);
			bool destroy();

			/**
			 * Sets the time of the last packet which the server definately got.
			 */
			void setAcknowledgedPacket(int time);
			/**
			 * Returns the time of the last received packet on the server.
			 */
			int getAcknowledgedPacket();

			bool update();

			void send(BufferPointer buffer, bool reliable = false);

			MapPointer getMap();
		private:
			Client();

			MapPointer map;

			ENetHost *host;
			ENetPeer *peer;

			int lastpacket;
	};
}

#endif
