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

#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include "ServerMap.hpp"
#include "Client.hpp"

#include <string>
#include <enet/enet.h>
#include <vector>

namespace backlot
{
	class Server
	{
		public:
			static Server &get();
			~Server();

			bool init(int port, std::string mapname, int maxclients = 8);
			bool destroy();

			MapPointer getMap()
			{
				return map.get();
			}

			void sendToAll(BufferPointer buffer, bool reliable = false);

			bool update();
		private:
			Server();

			ServerMapPointer map;
			std::string mapname;

			ENetHost *host;

			std::vector<Client*> clients;
	};
}

#endif
