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
#include "Player.hpp"

#include <GL/gl.h>
#include <iostream>
#include <cstdio>
#include <string>

namespace backlot
{
	HUDElement::HUDElement()
	{
		image = new Texture();
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
			std::cerr << "Unknown HUD element type: \"" << typestring << "\"\n";
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

		if (xmlelement->Attribute("backgroundcolor") != 0)
		{
			const char *backgroundcolorstring = xmlelement->Attribute("backgroundcolor") + 1;
 			backgroundcolor = std::strtoul(backgroundcolorstring, NULL, 16);
		}	
		else
			backgroundcolor = 0x0000007F;

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

 			if (xmlelement->Attribute("color") != 0)
			{
 				const char *fontcolorstring = xmlelement->Attribute("color") + 1;
				fontcolor = std::strtoul(fontcolorstring, NULL, 16);
			}
			else
				fontcolor = 0xFFFFFFFF;
		}
		if (xmlnode->FirstChild("image"))
		{
			childnode = xmlnode->FirstChild("image");
			xmlelement = childnode->ToElement();
			if (xmlelement->Attribute("path") == 0)
			{
				std::cerr << "No image path found for HUD element.\n";
				return false;
			}
 			image->load(xmlelement->Attribute("path"));
			if (xmlelement->Attribute("size") == 0)
			{
				std::cerr << "No image size for HUD element given.\n";
				return false;
			}
			imagesize = xmlelement->Attribute("size");
			if (xmlelement->Attribute("offset") == 0)
				imageoffset = Vector2F(0, 0);
			else
				imageoffset = xmlelement->Attribute("offset");
		}

		return true;
	}

	void HUDElement::render()
	{
		Vector2I resolution = Preferences::get().getResolution();
		float red = (float)(backgroundcolor >> 24) / 255;
		float green = (float)((backgroundcolor & 0x00FF0000) >> 16) / 255;
		float blue = (float)((backgroundcolor & 0x0000FF00) >> 8) / 255;
		float alpha = (float)(backgroundcolor & 0x000000FF) / 255;
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
 		glLoadIdentity();
 		glOrtho(0, resolution.x, resolution.y, 0, -1, 1);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(red, green, blue, alpha);
 		glTranslatef(offset.x, offset.y, 0);
		glBegin(GL_QUADS);
			glVertex3f((position.x * resolution.x), (position.y * resolution.y), 0);
			glVertex3f((position.x * resolution.x + size.x), (position.y * resolution.y), 0);
			glVertex3f((position.x * resolution.x + size.x), (position.y * resolution.y + size.y), 0);
			glVertex3f((position.x * resolution.x), (position.y * resolution.y + size.y), 0);
		glEnd();
		glDisable(GL_BLEND);
		glColor3f(1.0, 1.0, 1.0);

		PlayerPointer localplayer = Player::getLocalPlayer();

		if (localplayer.isNull() == false)
		{
			if (type == EHET_Health)
			{
				char message[8];
				std::snprintf(message, 8, "%d %%", localplayer->getHitpoints());
				if (font)
					font->render(message, Vector2F((position.x * resolution.x) + fontoffset.x,
						(position.y * resolution.y) + fontoffset.y), fontsize, (int)fontcolor);
			}
			if (type == EHET_Ammo)
			{
				if (font)
				{
					char message[16];
					// How many rounds are left in the current magazine.
					int currentmagazine = localplayer->getCurrentWeapon()->currentmagazine;
					// How many rounds are in one magazine.
					int magazinesize = localplayer->getCurrentWeapon()->weapon->getMagazineSize();
					// How many unused rounds the player still has.
					int totalrounds = localplayer->getCurrentWeapon()->reserve;
					std::snprintf(message, 16, "%d / %d (%d)", currentmagazine, magazinesize, totalrounds);

					font->render(message, Vector2F((position.x * resolution.x) + fontoffset.x,
						(position.y * resolution.y) + fontoffset.y), fontsize, fontcolor);
				}
			}
		}

		// Render the image
		if (image)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			image->bind();
			glEnable(GL_TEXTURE_2D);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0);
				glVertex3f((position.x * resolution.x) + imageoffset.x, (position.y * resolution.y) + imageoffset.y, 0);
				glTexCoord2f(1.0, 0.0);
				glVertex3f((position.x * resolution.x + imagesize.x) + imageoffset.x, (position.y * resolution.y) + imageoffset.y, 0);
				glTexCoord2f(1.0, 1.0);
				glVertex3f((position.x * resolution.x + imagesize.x) + imageoffset.x, (position.y * resolution.y + imagesize.y) + imageoffset.y, 0);
				glTexCoord2f(0.0, 1.0);
				glVertex3f((position.x * resolution.x) + imageoffset.x, (position.y * resolution.y + imagesize.y) + imageoffset.y, 0); 
			glEnd();
			glDisable(GL_TEXTURE_2D);
		}

		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
	}
}
