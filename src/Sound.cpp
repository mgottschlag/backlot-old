/*
Copyright (C) 2009  Simon Kerler

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

#include "Sound.hpp"
#include "Engine.hpp"

#include <iostream>

namespace backlot
{
	Sound::Sound() : ReferenceCounted()
	{
		sound = NULL;
	}
	Sound::~Sound()
	{
	}
	
	bool Sound::load(std::string path)
	{
		path = Engine::get().getGameDirectory() + "/sounds/" + path;
		sound = Mix_LoadWAV(path.c_str());
		if (sound == NULL)
		{
			std::cerr << "Failed to load soundfile \"" << path << "\"\n";
			return false;
		}
		return true;
	}
	
	bool Sound::play()
	{
		// If a sound file was loaded
		if (sound != NULL)
		{
			if (Mix_PlayChannel(-1, sound, 0) != 0)
			{
				std::cerr << "Failed to start sound playback\n";
				return false;
			}
		}
		else
		{
			std::cerr << "No soudfile was loaded\n";
			return false;
		}
		return true;
	}
	bool Sound::stop()
	{
		return true;
	}
	bool Sound::pause()
	{
		return true;
	}
	bool Sound::resume()
	{
		return true;
	}

}
