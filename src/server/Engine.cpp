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

#include "Engine.hpp"
#include "Preferences.hpp"
#include "Server.hpp"
#include "PathFinder.hpp"

#include <iostream>
#include <fstream>
#include <enet/enet.h>
#include <sys/stat.h> 

namespace backlot
{
	Engine &Engine::get()
	{
		static Engine engine;
		return engine;
	}
	Engine::~Engine()
	{
	}

	bool Engine::run(std::string path, std::vector<std::string> args)
	{
		stopping = false;
		directory = path;

		//Check to see if the Gamedir exist
		struct stat fileinfo;
		if (stat(getGameDirectory().c_str(), &fileinfo))
		{
			std::cerr << "Game directory does not exist!" << std::endl;
			return false;
		}
		
		// Load main configuration file
		Preferences::get().setPath(getGameDirectory() + "/config.xml");
		if (!Preferences::get().load())
		{
			std::cerr << "error: Could not load preferences." << std::endl;
			return false;
		}
		// Show configuration dialog
		// Start engine
		if (enet_initialize() != 0)
		{
			std::cerr << "error: Could not initialize networking." << std::endl;
			return false;
		}
		
		int port = 27272;
		std::string mapname = "test";
		
		// Parse command line arguments
		for (int i = 0; i < int(args.size()); i++)
		{
			std::string option = args[i];
			if ( ( (option == "--map") || (option == "-m") ))
			{
				i++;
				mapname = args[i];
			}
			if ( ( (option == "--port") || (option == "-p") ))
			{
				i++;
				port = atoi(args[i].c_str());
			}
		}
		
		// Start server
		if (!Server::get().init(port, mapname))
		{
			return false;
		}
		// Main loop
		std::cout << "ready" << std::endl;
		static std::ofstream fout;
		fout.open("./server.log");
		std::cout.rdbuf(fout.rdbuf());
		std::cerr.rdbuf(fout.rdbuf());
		std::cout << "Server started." << std::endl;
		lastframe = getTime();
		while (!stopping)
		{
			// Game logic
			if (!Server::get().update())
				stopping = true;
			PathFinder::updateAll();
			// Input handling
			// TODO
			// Fixed time step
			lastframe = lastframe + 20000;
			uint64_t currenttime = getTime();
			if (currenttime < lastframe)
				usleep(lastframe - currenttime);
		}
		// Shut down engine
		enet_deinitialize();
		return true;
	}

	std::string Engine::getGameDirectory()
	{
		return directory;
	}

	void Engine::stop()
	{
		stopping = true;
	}

	Engine::Engine()
	{
	}
}
