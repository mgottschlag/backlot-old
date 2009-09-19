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

#ifndef _GRAPHICS_HPP_
#define _GRAPHICS_HPP_

#include "graphics/Camera.hpp"

namespace gcn
{
	class Gui;
	class SDLImageLoader;
	class OpenGLGraphics;
	class Container;
	class SDLInput;
}

namespace backlot
{
	class GuichanFont;

	class Graphics
	{
		public:
			static Graphics &get();
			~Graphics();

			bool init(int width, int height, int bpp, bool fullscreen);
			bool destroy();

			CameraPointer getCamera();

			bool render();

			gcn::Container *getGuichanContainer()
			{
				return top;
			}
			gcn::SDLInput *getGuichanInput()
			{
				return input;
			}
		private:
			Graphics();

			CameraPointer camera;

			// guichan classes
			gcn::SDLImageLoader *imageloader;
			gcn::OpenGLGraphics *menugraphics;
			gcn::Gui *gui;
			gcn::Container *top;
			GuichanFont *font;
			gcn::SDLInput *input;

			// FPS counter
			unsigned int frames_rendered;
			unsigned int last_ticks;
	};
}

#endif
