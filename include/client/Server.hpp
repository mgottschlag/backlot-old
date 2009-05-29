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

#include "Map.hpp"

#include <string>
#include <enet/enet.h>
#include <cstdio>

namespace backlot
{
	/**
	 * Interface to the server executable. This class starts an external server
	 * executable and communicates with it through pipes.
	 */
	class Server
	{
		public:
			/**
			 * Returns a reference to the server interface.
			 */
			static Server &get();
			/**
			 * Destructor.
			 */
			~Server();

			/**
			 * Starts the server.
			 */
			bool init(int port, std::string mapname, int maxclients = 8);
			/**
			 * Kills the server.
			 */
			bool destroy();

			bool update();
		private:
			Server();

			#if defined(_MSC_VER) || defined(_WINDOWS_) || defined(_WIN32)
			#else
			int in;
			FILE *infile;
			int out;
			int pid;
			#endif
	};
}

#endif
