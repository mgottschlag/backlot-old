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

#include "Music.hpp"
#include "Engine.hpp"

#include <iostream>

namespace backlot
{
	Music::Music() : ReferenceCounted()
	{
		music = NULL;
	}
	Music::~Music()
	{
		if (currentmusic == this)
			currentmusic = NULL;
		Mix_FreeMusic(music);
	}
	
	Music *Music::getCurrentMusic()
	{
		if (currentmusic == NULL)
			std::cerr << "In " << __FILE__ << __LINE__ << ": No music is played. Returning NULL.\n";
		return currentmusic;
	}

	bool Music::load(std::string path)
	{
		path = Engine::get().getGameDirectory() + "/music/" + path;
		music = Mix_LoadMUS(path.c_str());
		if (music == NULL)
		{
			std::cerr << "Failed to load musicfile \"" << path << "\"\n";
			return false;
		}
		return true;
	}

	bool Music::play()
	{
		// If a music file was loaded
		if (music != NULL)
		{
			// If no other music is played or paused
			if (Mix_PlayingMusic() == 0 && currentmusic == NULL)
			{
				if (Mix_PlayMusic(music, -1) != 0)
				{
					std::cerr << "Failed to play music\n";
					return false;
				}
				currentmusic = this;
			}
			// If an other music file is played
			else
			{
				std::cerr << "Already playing music. Don't start second playback.\n";
				return false;
			}
		}
		// If no musicfile was loaded
		else
		{
			std::cerr << "No music file was loaded!\n";
			return false;
		}
		return true;
	}
	bool Music::stop()
	{
		if (Mix_PlayingMusic() == 1)
		{
			// If this music is played
			if (currentmusic == this)
			{
				Mix_HaltMusic();
				currentmusic = NULL;
			}
			// If an other music is played
			else
			{
				std::cerr << "An other music file is played. Stop that one!\n";
				return false;
			}
		}
		// If no music is played
		else
		{
			std::cerr << "No music is played. Can't stop playback (how could I?)\n";
			return false;
		}
		return true;
	}
	bool Music::pause()
	{
		if (Mix_PlayingMusic() == 1)
		{
			if (currentmusic == this)
				Mix_PauseMusic();
			// If an other music is played
			else
			{
				std::cerr << "An other music instance is playing. Pause that one!\n";
				return false;
			}
		}
		// If no music is playing
		else
		{
			std::cerr << "No music is played. Can't pause playback.\n";
			return false;
		}
		return true;
	}
	bool Music::resume()
	{
		if (Mix_PausedMusic() == 1)
		{
			if (currentmusic == this)
				Mix_ResumeMusic();
			// If an other music is paused
			else
			{
				std::cerr << "An other music is paused. Resume that one!\n";
				return false;
			}
		}
		// If no music is paused
		else
		{
			std::cerr << "No music is paused. Can't resume playback.\n";
			return false;
		}
		return true;
	}

	// Set the static pointer to the current played music to NULL.
	Music *Music::currentmusic = NULL;
}