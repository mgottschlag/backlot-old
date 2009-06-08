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

#include "HUDElement.hpp"
#include "Preferences.hpp"

#include <GL/gl.h>
#include <iostream>
#include <string>

namespace backlot
{
	HUDElement::HUDElement()
	{
	}
	HUDElement::~HUDElement()
	{
	}

	bool HUDElement::load(TiXmlNode *xmlnode)
	{
		TiXmlNode *childnode = NULL;
		TiXmlElement *xmlelement = xmlnode->ToElement();

		// Get the type of the HUD element.
		if (xmlelement->Attribute("type") == 0)
		{
			std::cerr << "No type information for HUD element given.\n";
			return false;
		}
		std::string typestring = xmlelement->Attribute("type");
		if (typestring.compare("health") == 0)
			type = EHET_Health;
		else if (typestring.compare("ammo") == 0)
			type = EHET_Ammo;
		else
		{
			std::cout << "Unknown HUD element type: \"" << typestring << "\"\n";
			return false;
		}

		if (xmlelement->Attribute("position") == 0)
		{
			std::cerr << "No position information for HUD element given.\n";
			return false;
		}
		position = xmlelement->Attribute("position");

		if (xmlelement->Attribute("offset") == 0)
		{
			std::cerr << "No offset information for HUD element given.\n";
			return false;
		}
		offset = xmlelement->Attribute("offset");

		if (xmlelement->Attribute("size") == 0)
		{
			std::cerr << "No size information for HUD element given.\n";
			return false;
		}
		size = xmlelement->Attribute("size");

		if (xmlnode->FirstChild("font"))
		{
			childnode = xmlnode->FirstChild("font");
			xmlelement = childnode->ToElement();
 			if (xmlelement->Attribute("name") == 0)
 			{
 				std::cerr << "No font name for the HUD element given.\n";
 				return false;
 			}
			font = Font::get(xmlelement->Attribute("name"));

			if (xmlelement->Attribute("offset") != 0)
				fontoffset = xmlelement->Attribute("offset");
			else
			{
				offset.x = 0;
				offset.y = 0;
			}

			if (xmlelement->Attribute("size") != 0)
				xmlelement->Attribute("size", &fontsize);
			else
				fontsize = 20;
		}

		return true;
	}

	void HUDElement::render()
	{
		Vector2I resolution = Preferences::get().getResolution();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
 		glLoadIdentity();
 		glOrtho(0, resolution.x, resolution.y, 0, -1, 1);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		glColor4f(0.5, 0.5, 0.5, 0.5);
 		glTranslatef(offset.x, offset.y, 0);
		glBegin(GL_QUADS);
			glVertex3f((position.x * resolution.x), (position.y * resolution.y), 0);
			glVertex3f((position.x * resolution.x + size.x), (position.y * resolution.y), 0);
			glVertex3f((position.x * resolution.x + size.x), (position.y * resolution.y + size.y), 0);
			glVertex3f((position.x * resolution.x), (position.y * resolution.y + size.y), 0);
		glEnd();
		glColor3f(1.0, 1.0, 1.0);
		glDisable(GL_BLEND);

		if (type == EHET_Health)
		{
			if (font != NULL)
				font->render("Health:", Vector2F((position.x * resolution.x) + fontoffset.x,
					(position.y * resolution.y) + fontoffset.y), fontsize);
		}
		if (type == EHET_Ammo)
		{
			if (font != NULL)
				font->render("Ammo:", Vector2F((position.x * resolution.x) + fontoffset.x,
					(position.y * resolution.y) + fontoffset.y), fontsize);
		}
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
	}
}
