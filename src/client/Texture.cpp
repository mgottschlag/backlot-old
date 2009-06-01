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

#include "Texture.hpp"
#include "Engine.hpp"

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>

namespace backlot
{
	Texture::Texture() : ReferenceCounted()
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	Texture::~Texture()
	{
		glDeleteTextures(1, &texture);
	}

	bool Texture::load(std::string path)
	{
		// Load image
		path = Engine::get().getGameDirectory() + "/" + path;
		SDL_Surface *surface = IMG_Load(path.c_str());
		if (!surface)
		{
			std::cerr << "Can't load image \"" << path <<"\". Does it exist?\n";
			return false;
		}
		SDL_PixelFormat *format = surface->format;
		// Set texture
		glBindTexture(GL_TEXTURE_2D, texture);
		SDL_LockSurface(surface);
		if (format->BytesPerPixel == 4)
			gluBuild2DMipmaps(GL_TEXTURE_2D, 4, surface->w, surface->h, GL_RGBA,
				GL_UNSIGNED_BYTE, surface->pixels);
		else
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, surface->w, surface->h, GL_RGB,
				GL_UNSIGNED_BYTE, surface->pixels);
		SDL_UnlockSurface(surface);
		// Delete image
		SDL_FreeSurface(surface);
		return true;
	}

	void Texture::bind()
	{
		glBindTexture(GL_TEXTURE_2D, texture);
	}
}
