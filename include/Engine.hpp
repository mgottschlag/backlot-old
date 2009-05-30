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

#ifndef _ENGINE_HPP_
#define _ENGINE_HPP_

#include <string>
#include <vector>

#if defined(_MSC_VER) || defined(_WINDOWS_) || defined(_WIN32)
#include <time.h>
#include <winsock2.h>
#include <winbase.h>
#define usleep(x) Sleep((x)/1000)
#else
#include <sys/time.h>
#include <netinet/in.h>
#endif

/**
 * @mainpage Backlot Engine
 *
 * @section about About
 * TBD
 */

/**
 * Main engine namespace.
 */
namespace backlot
{
	/**
	 * Main engine class. Loads and manages the other parts of the engine.
	 */
	class Engine
	{
		public:
			/**
			 * Returns a reference to the engine.
			 */
			static Engine &get();
			/**
			 * Destructor.
			 */
			~Engine();

			/**
			 * Initializes the engine and executes the game at the specified
			 * path.
			 * @param path Path to the game data. Can be either absolute or
			 * relative to the current working directory.
			 * @param args Additional command line arguments.
			 * @return Returns false if there have been any errors.
			 */
			bool run(std::string path,
				std::vector<std::string> args = std::vector<std::string>());

			/**
			 * Returns the path to the game directory.
			 */
			std::string getGameDirectory();

			static uint64_t getTime()
			{
				#if defined(_MSC_VER) || defined(_WINDOWS_) || defined(_WIN32)
				return (uint64_t)GetTickCount() * 1000;
				#else
				struct timeval tv;
				gettimeofday(&tv, 0);
				return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
				#endif
			}
		private:
			/**
			 * Constructor.
			 */
			Engine();

			std::string directory;

			uint64_t lastframe;
	};
}

#endif
