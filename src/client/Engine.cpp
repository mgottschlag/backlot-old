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
#include "menu/Menu.hpp"
#include "menu/Dialog.hpp"
#include "SplashScreen.hpp"
#include "support/tinyxml.h"

#include <SDL/SDL.h>
#include <iostream>
#include <sys/stat.h> 

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
		if (!Audio::get().init(Preferences::get().getFrequency(),
			Preferences::get().getStereo()))
		{
			std::cerr << "Could not initialize sound." << std::endl;
			return false;
		}
		// Show splash screens
		SplashScreen::showAll();
		// Show main menu
		std::string filename = Engine::get().getGameDirectory() + "/game.xml";
		TiXmlDocument xml(filename.c_str());
		if (!xml.LoadFile() || xml.Error())
		{
			std::cerr << "Could not load XML file game.xml: " << xml.ErrorDesc() << std::endl;
			return false;
		}
		TiXmlNode *node = xml.FirstChild("game");
		if (!node)
		{
			std::cerr << "Parser error: <game> not found." << std::endl;
			return false;
		}
		TiXmlElement *root = node->ToElement();
		MenuPointer mainmenu;
		if (!root->Attribute("startmenu"))
			mainmenu = Menu::get(root->Attribute("startmenu"));
		else
			mainmenu = Menu::get("main");
		if (mainmenu)
		{
			mainmenu->setActive(true);
		}

		// Start server or connect to one
		std::string serveraddress = "localhost:27272";
		bool startserver = false;
		bool connect = false;
		
		for (int i = 0; i < int(args.size()); i++)
		{
			std::string option = args[i];
			if (((option == "--host") || (option == "-h")))
			{
				startserver = true;
				connect = true;
			}
			if (((option == "--join") || (option == "-j")))
			{
				i++;
				serveraddress = args[i];
				startserver = false;
				connect = true;
			}
		}
		if (startserver)
		{
			if (!Server::get().init(27272, "test"))
			{
				return false;
			}
		}
		if (connect)
		{
			if (!Client::get().init(serveraddress))
			{
				return false;
			}
		}

		// Main loop
		lastframe = getTime();
		while (!stopping)
		{
			// Input
			if (!Input::get().update())
				stopping = true;
			// Game logic
			Server::get().update();
			Client::get().update();
			// Render everything
			if (!Graphics::get().render())
				stopping = true;
			// Fixed time step
			lastframe = lastframe + 20000;
			uint64_t currenttime = getTime();
			if (currenttime < lastframe)
				usleep(lastframe - currenttime);
		}
		
		Client::get().destroy();
		Server::get().destroy();
		// Shut down engine
		Dialog::unloadAll();
		Audio::get().destroy();
		Graphics::get().destroy();
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
