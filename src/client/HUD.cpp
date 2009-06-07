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

#include "HUD.hpp"
#include "Engine.hpp"
#include "Preferences.hpp"
#include "tinyxml.h"

#include <GL/gl.h>
#include <iostream>
#include <string>

namespace backlot
{
	HUD &HUD::get()
	{
		static HUD hud;
		return hud;
	}
	HUD::~HUD()
	{
	}

	bool HUD::load()
	{
		std::string configfilepath = Engine::get().getGameDirectory() + "/" + "hud.xml";
		std::cout << "Loading HUD from \"" << configfilepath << "\"\n";
		TiXmlDocument configfile(configfilepath.c_str());
		if (!configfile.LoadFile())
		{
			std::cerr << "Could not load config file for HUD from \"" << configfilepath << "\"\n";
			return false;
		}

		TiXmlNode *node;
		TiXmlElement *element;

		if ((node = configfile.FirstChild("hud")) == NULL)
		{
			std::cerr << "Could not find \"hud\" in \"" << configfilepath << "\"\n";
			return false;
		}
		if ((node = node->FirstChild("health")) == NULL)
		{
			std::cerr << "Could not find \"health\" in \"hud\" in \"" << configfilepath << "\"\n";
			return false;
		}
		element = node->ToElement();
		if (element->Attribute("position") == 0)
		{
			std::cerr << "No position information for HUD element \"<health>\" given.\n";
			return false;
		}
		healthelement.position = element->Attribute("position");
		if (element->Attribute("offset") == 0)
		{
			std::cerr << "No offset information for HUD element \"<healt>\" given.\n";
			return false;
		}
		healthelement.offset = element->Attribute("offset");
		// Convert the pixle offset to offset relative to the OGl coordinate system.
		healthelement.offset.x = (1.0 / Preferences::get().getResolution().x) * healthelement.offset.x;
		healthelement.offset.y = (1.0 / Preferences::get().getResolution().y) * healthelement.offset.y;
		if (element->Attribute("size") == 0)
		{
			std::cerr << "No size information for HUD element \"<health>\" given.\n";
			return false;
		}
		healthelement.size = element->Attribute("size");
		// Convert the pixle size to a relative size.
		healthelement.size.x = (1.0 / Preferences::get().getResolution().x) * healthelement.size.x;
		healthelement.size.y = (1.0 / Preferences::get().getResolution().y) * healthelement.size.y;
		if (element->Attribute("font") == 0)
		{
			std::cerr << "No font information given for HUD element \"<health>\".\n";
			return false;
		}
		healthelement.font = Font::get(element->Attribute("font"));

		return true;
	}

	void HUD::render()
	{
		Vector2F relativeoffset;
		Vector2F relativesize;
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
 		glLoadIdentity();
 		glOrtho(0, 1, 1, 0, -1, 1);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		glColor4f(0.5, 0.5, 0.5, 0.5);
		glTranslatef(healthelement.offset.x, healthelement.offset.y, 0);
		glBegin(GL_QUADS);
			glVertex3f(healthelement.position.x, healthelement.position.y, 0);
			glVertex3f(healthelement.position.x + healthelement.size.x,
				healthelement.position.y, 0);
			glVertex3f(healthelement.position.x + healthelement.size.x,
				healthelement.position.y + healthelement.size.y, 0);
			glVertex3f(healthelement.position.x,
				healthelement.position.y + healthelement.size.y, 0);
		glEnd();
		glColor3f(1.0, 1.0, 1.0);
		glDisable(GL_BLEND);
		healthelement.font->render("Health", healthelement.position, healthelement.size);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
	}

	HUD::HUD()
	{
	}
}
