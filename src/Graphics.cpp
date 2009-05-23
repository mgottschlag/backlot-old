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

#include "Graphics.hpp"

#include <SDL/SDL.h>
#include <iostream>

namespace backlot
{
	Graphics &Graphics::get()
	{
		static Graphics graphics;
		return graphics;
	}
	Graphics::~Graphics()
	{
	}

	bool Graphics::init(int width, int height, int bpp, bool fullscreen)
	{
		// Initialize SDL
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		{
			std::cerr << "Could not initialize SDL." << std::endl;
			return false;
		}
		// Create OpenGL context
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		int flags = SDL_OPENGL;
		if (fullscreen)
			flags |= SDL_FULLSCREEN;
		if (SDL_SetVideoMode(width, height, 0, flags) < 0)
		{
			std::cerr << "Could not set video mode." << std::endl;
			return false;
		}
		SDL_WM_SetCaption("Backlot Engine", "Backlot Engine");
		frames_rendered = 0;
		last_ticks = SDL_GetTicks();
		return true;
	}
	bool Graphics::destroy()
	{
		SDL_Quit();
		return true;
	}

	bool Graphics::render()
	{
		// FPS counter
		frames_rendered++;
		if (frames_rendered == 10)
		{
			unsigned int ticks = SDL_GetTicks();
			frames_rendered = 0;
			int fps = 1000 / (ticks - last_ticks) * 10;
			last_ticks = ticks;
			char caption[100];
			snprintf(caption, 100, "Backlot Engine - %d fps", fps);
			SDL_WM_SetCaption(caption, caption);
		}
		// Render everything
		// Swap buffers
		SDL_GL_SwapBuffers();
		return true;
	}

	Graphics::Graphics()
	{
	}
}
