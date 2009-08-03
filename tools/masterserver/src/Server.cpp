/*
Copyright (C) 2009 Stephan Jauernick

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

#include "Server.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>

namespace backlot
{
	Server::Server()
	{
		id = 0;
		ip = "127.0.0.1";
		port = 27272;
		players = 0;
		maxplayers = 8;
		map = "test";
	}

	Server::~Server()
	{
	}

	int Server::print_infos()
	{
		std::cout << "Server: " << this->id << " IP:" << this->ip << ":" << this->port << " Players:" << this->players << "/" << this->maxplayers << " Map:" << this->map << std::endl;
		return 1;
	}
	
	std::string Server::get_path()
	{
		std::stringstream buf;
		buf << port;
		return ip + ":" + buf.str();
	}

	int Server::ping()
	{
		std::cout << "PING IP/File:" << get_path() << std::endl;
		std::ifstream pingfile;
		pingfile.open (get_path().c_str(), std::ifstream::in);
		
		if (pingfile.is_open())
		{
			while (pingfile.good())
			{
				std::string line;
				pingfile >> line;

				if (((line == "ip")))
				{
					pingfile >> line;
					ip = line;
				}
				if (((line == "port")))
				{
					pingfile >> line;
					port = atoi(line.c_str());
				}
				if (((line == "players")))
				{
					pingfile >> line;
					players = atoi(line.c_str());
				}
				if (((line == "maxplayers")))
				{
					pingfile >> line;
					maxplayers = atoi(line.c_str());
				}
				if (((line == "map")))
				{
					pingfile >> line;
					map = line;
				}
			}
			pingfile.close();
			return 1;
		}
		else
		{
			return 0;
		}

		return 0;
	}
}
