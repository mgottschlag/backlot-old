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

#include "SplashScreen.hpp"
#include "Engine.hpp"
#include "Preferences.hpp"

#include "support/tinyxml.h"

#include <vector>
#include <iostream>
#include <GL/gl.h>
#include <SDL.h>

namespace backlot
{
	bool SplashScreen::showAll()
	{
		std::vector<SplashScreen*> screens;
		// Open XML file
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
		// Load splash screens
		TiXmlNode *splashnode = root->FirstChild("splash");
		while (splashnode)
		{
			TiXmlElement *splashdata = splashnode->ToElement();
			if (splashdata)
			{
				// Attributes
				if (!splashdata->Attribute("image"))
				{
					std::cerr << "No splash screen image!" << std::endl;
					splashnode = node->IterateChildren("splash", splashnode);
					continue;
				}
				std::string texture = splashdata->Attribute("image");
				int index = 0;
				if (splashdata->Attribute("index"))
					splashdata->Attribute("index", &index);
				double time = 5.0;
				if (splashdata->Attribute("time"))
					splashdata->Attribute("time", &time);
				bool input = true;
				if (splashdata->Attribute("input")
					&& !strcmp(splashdata->Attribute("input"), "no"))
					input = false;
				// Load splash screen
				SplashScreen *screen = new SplashScreen();
				if (!screen->load(texture, time, input, index))
				{
					std::cerr << "Could not load splash screen." << std::endl;
					splashnode = node->IterateChildren("splash", splashnode);
					continue;
				}
				// Insert screen into the list
				bool inserted = false;
				for (unsigned int i = 0; i < screens.size(); i++)
				{
					if (screens[i]->getIndex() > index)
					{
						screens.insert(screens.begin() + i, screen);
						inserted = true;
						break;
					}
				}
				if (!inserted)
					screens.push_back(screen);
			}
			splashnode = node->IterateChildren("splash", splashnode);
		}
		// Show images
		for (unsigned int i = 0; i < screens.size(); i++)
		{
			screens[i]->show();
		}
		// Clean up
		for (unsigned int i = 0; i < screens.size(); i++)
		{
			delete screens[i];
		}
		screens.clear();
		return true;
	}

	SplashScreen::SplashScreen()
	{
	}
	SplashScreen::~SplashScreen()
	{
	}

	bool SplashScreen::load(std::string texturepath, float time, bool input,
		int index)
	{
		texture = Texture::get(texturepath);
		if (!texture)
			return false;
		this->time = time;
		this->input = input;
		this->index = index;
		return true;
	}
	int SplashScreen::getIndex()
	{
		return index;
	}

	bool SplashScreen::show()
	{
		// Clear
		// TODO: Configurable color
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Center image
		Vector2I screensize = Preferences::get().getResolution();
		Vector2I imagesize = texture->getSize();
		Vector2I position = screensize / 2 - imagesize / 2;
		// Draw image
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, screensize.x, screensize.y, 0, -1, 1);
		texture->bind();
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0);
			glVertex2i(position.x, position.y);
			glTexCoord2f(1.0, 0.0);
			glVertex2i(position.x + imagesize.x, position.y);
			glTexCoord2f(1.0, 1.0);
			glVertex2i(position.x + imagesize.x, position.y + imagesize.y);
			glTexCoord2f(0.0, 1.0);
			glVertex2i(position.x, position.y + imagesize.y);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
		// Swap buffers
		SDL_GL_SwapBuffers();
		// Wait
		unsigned int usecs = time * 1000000;
		for (unsigned int i = 0; i < usecs / 100000; i++)
		{
			// Handle SDL events
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				switch(event.type)
				{
					case SDL_QUIT:
						return false;
					case SDL_KEYDOWN:
					case SDL_MOUSEBUTTONDOWN:
						return true;
					default:
						break;
				}
			}
			usleep(100000);
		}
		return true;
	}
}