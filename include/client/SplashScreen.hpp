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

#ifndef _SPLASHSCREEN_HPP_
#define _SPLASHSCREEN_HPP_

#include "graphics/Texture.hpp"

namespace backlot
{
	/**
	 * Class for static full screen splash screens at the beginning of the game.
	 * Splash screens are defined in the game.xml file in the game directory.
	 */
	class SplashScreen
	{
		public:
			/**
			 * Loads and shows all splash screens. Actually this would be the
			 * only function called from outside this class.
			 */
			static bool showAll();

			/**
			 * Constructor.
			 */
			SplashScreen();
			/**
			 * Destructor.
			 */
			~SplashScreen();

			/**
			 * Generates the splash screen from the given texture file.
			 * @param texturepath Path to the texture file relative to the game
			 * directory.
			 * @param time Time in seconds for the splash screen to last.
			 * @param input If set to true, the user can cancel the splash
			 * screen with any key or mouse button.
			 * @param index Sets the index of the splash screen. Screens with
			 * a smaller index are called first.
			 */
			bool load(std::string texturepath, float time, bool input,
				int index);
			/**
			 * Returns the index of the splash screen.
			 */
			int getIndex();

			/**
			 * Shows the splash screen, waits for either user input (if enabled)
			 * or the timeout to occur and hides the splash screen again.
			 */
			bool show();
		private:
			/**
			 * Background texture.
			 */
			TexturePointer texture;
			/**
			 * Time in seconds for the splash screen to be shown.
			 */
			float time;
			/**
			 * If set to true, the splash screen is cancellable.
			 */
			bool input;
			/**
			 * Index of the splash screen.
			 */
			int index;
	};
}

#endif
