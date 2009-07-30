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

#include "Game.hpp"

#include <iostream>

#ifdef unix
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#else
#include <windows.h>
#endif

Game &Game::get()
{
	static Game game;
	return game;
}

bool Game::init(std::string path)
{
	// TODO
	this->path = path;
	return true;
}
std::string Game::getPath()
{
	return path;
}

std::vector<std::string> Game::getMaps()
{
	// Get file list
	std::vector<std::string> tilesets = getDirectoryList("maps", ".blm");
	// Strip away extensions
	for (unsigned int i = 0; i < tilesets.size(); i++)
	{
		tilesets[i] = tilesets[i].substr(0, tilesets[i].size() - 4);
	}
	return tilesets;
}
std::vector<std::string> Game::getTileSets()
{
	// Get file list
	std::vector<std::string> tilesets = getDirectoryList("tilesets", ".xml");
	// Strip away extensions
	for (unsigned int i = 0; i < tilesets.size(); i++)
	{
		tilesets[i] = tilesets[i].substr(0, tilesets[i].size() - 4);
	}
	return tilesets;
}
std::vector<std::string> Game::getEntities()
{
	// Get file list
	std::vector<std::string> entities = getDirectoryList("entities", ".xml");
	// Strip away extensions
	for (unsigned int i = 0; i < entities.size(); i++)
	{
		entities[i] = entities[i].substr(0, entities[i].size() - 4);
	}
	return entities;
}

Game::Game()
{
}

std::vector<std::string> Game::getDirectoryList(std::string directory,
	std::string extension)
{
	std::vector<std::string> files;
	#ifdef __unix__
	std::string fulldirname = path + "/" + directory;
	DIR *dir = opendir(fulldirname.c_str());
	if (!dir)
	{
		std::cerr << "Could not read directory \"" << fulldirname.c_str() << "\"." << std::endl;
		return std::vector<std::string>();
	}

	while (1)
	{
		struct dirent *dirent = readdir(dir);
		if (!dirent) break;
		if (strcmp(dirent->d_name, "..") && strcmp(dirent->d_name, "."))
		{
			std::string filename = fulldirname + "/" + dirent->d_name;
			int file = open(filename.c_str(), O_RDONLY);
			if (!file)
			{
				std::cerr << "Could not open file \"" << filename.c_str() << "\"." << std::endl;
				continue;
			}
			struct stat stat;
			fstat(file, &stat);
			close(file);

			if (stat.st_mode & S_IFREG)
			{
				std::string ext = filename.substr(filename.size() - extension.size(), extension.size());
				if (ext == extension)
				{
					files.push_back(dirent->d_name);
				}
			}
		}
	}
	closedir(dir);

	#else
	std::string pattern = path + "/" + directory + "/*" + extension;
	WIN32_FIND_DATA finddata;
	HANDLE findhandle = FindFirstFile(pattern.c_str(), &finddata);
	if (findhandle == INVALID_HANDLE_VALUE)
	{
		return files;
	}
	files.push_back(finddata.cFileName);
	while (FindNextFile(findhandle, &finddata))
	{
		files.push_back(finddata.cFileName);
	}
	FindClose(findhandle);
	#endif
	return files;
}
