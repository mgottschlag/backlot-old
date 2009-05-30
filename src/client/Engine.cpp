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
#include "Graphics.hpp"
#include "Input.hpp"
#include "Audio.hpp"
#include "Sound.hpp"
#include "Music.hpp"
#include "Server.hpp"
#include "Client.hpp"

#include <SDL/SDL.h>
#include <iostream>

#if defined(_MSC_VER) || defined(_WINDOWS_) || defined(_WIN32)
int gettimeofday(struct timeval *tv, void *timezone)
{
	union {
		long long ns100;
		FILETIME ft;
	} now;

	GetSystemTimeAsFileTime (&now.ft);
	tv->tv_usec = (long) ((now.ns100 / 10LL) % 1000000LL);
	tv->tv_sec = (long) ((now.ns100 - 116444736000000000LL) / 10000000LL);
	return (0);
}
#endif

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
		directory = path;
		// Load main configuration file
		Preferences::get().setPath(getGameDirectory() + "/config.xml");
		if (!Preferences::get().load())
		{
			std::cerr << "Could not load preferences." << std::endl;
			return false;
		}
		// Show configuration dialog
		// Start engine
		if (enet_initialize() != 0)
		{
			std::cerr << "Could not initialize networking." << std::endl;
			return false;
		}
		if (!Graphics::get().init(Preferences::get().getResolution().x,
			Preferences::get().getResolution().y,
			Preferences::get().getColorDepth(),
			Preferences::get().getFullscreen()))
		{
			std::cerr << "Could not open render window." << std::endl;
			return false;
		}
		if (!Audio::get().init())
		{
			std::cerr << "Could not initialize sound." << std::endl;
			return false;
		}
		// Show main menu
		// TODO
		// Start server
		if (!Server::get().init(27272, "test"))
		{
			return false;
		}
		// Start client
		if (!Client::get().init("localhost:27272"))
		{
			return false;
		}
		// Main loop
		bool running = true;
		lastframe = getTime();
		while (running)
		{
			// Input
			if (!Input::get().update())
				running = false;
			// Game logic
			Server::get().update();
			Client::get().update();
			// Render everything
			if (!Graphics::get().render())
				running = false;
			// Fixed time step
			lastframe = lastframe + 20000;
			uint64_t currenttime = getTime();
			if (currenttime < lastframe)
				usleep(lastframe - currenttime);
		}
		Client::get().destroy();
		Server::get().destroy();
		// Shut down engine
		Audio::get().destroy();
		Graphics::get().destroy();
		enet_deinitialize();
		return true;
	}

	std::string Engine::getGameDirectory()
	{
		return directory;
	}

	Engine::Engine()
	{
	}
}
