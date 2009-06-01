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

#ifndef _PREFERENCES_HPP_
#define _PREFERENCES_HPP_

#include "Vector2.hpp"

#include <string>

namespace backlot
{
	class Preferences
	{
		public:
			/**
			 * Returns a reference to the preferences.
			 */
			static Preferences &get();
			
			~Preferences();
			
			/*
			 * Loads the preferences from game/config.xml.
			 */
			bool load();
			/*
			 * Save the preferences to game/config.xml.
			 */
			bool save();
			
			void setPath(std::string newpath);
			/**
			 * Get the screenresolutions.
			 * @return A Vector2I with the width (x) and height (y) of the screen.
			 */
			const Vector2I &getResolution() const;
			int getColorDepth() const;
			/**
			 * If the game runs in fullscreen mode or not.
			 * @return True if fullscreen mode is enabled and false if it is disabled.
			 */
			bool getFullscreen() const;
			
			/**
			 * If the sound is stereo or not.
			 * @retrun True if stereo, false if mono.
			 */
			bool getStereo() const;
			int getFrequency() const;
			int getBitrate() const;
		private:
			/**
			 * The private constructor.
			 */
			 Preferences();
			 
			 std::string path;
			 
			 Vector2I screenresolution;
			 int colordepth;
			 bool fullscreen;
			 
			 bool stereo;
			 int frequency;
			 int bitrate;
	};
}
#endif
