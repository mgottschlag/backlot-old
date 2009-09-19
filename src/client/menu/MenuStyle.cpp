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

#include "menu/MenuStyle.hpp"

#include "support/tinyxml.h"

namespace backlot
{
	MenuStyle::MenuStyle() : ReferenceCounted()
	{
		buttonstyle = EBS_Normal;
	}
	MenuStyle::~MenuStyle()
	{
	}

	bool MenuStyle::load(TiXmlElement *xml)
	{
		// Read name
		if (xml->Attribute("name"))
			name = xml->Attribute("name");
		else
			name = "default";
		// Read button style
		TiXmlNode *buttonnode = xml->FirstChild("button");
		while (buttonnode)
		{
			TiXmlElement *button = buttonnode->ToElement();
			if (button)
			{
				if (button->Attribute("type"))
					if (!strcmp(button->Attribute("type"), "simple"))
						buttonstyle = EBS_Simple;
				break;
			}
			buttonnode = xml->IterateChildren("button", buttonnode);
		}
		// Read colors
		TiXmlNode *colornode = xml->FirstChild("color");
		while (colornode)
		{
			TiXmlElement *color = colornode->ToElement();
			if (color)
			{
				if (color->Attribute("background"))
					bgcolor = toColor(color->Attribute("background"));
				if (color->Attribute("normal"))
					textcolor = toColor(color->Attribute("normal"));
				if (color->Attribute("active"))
					activecolor = toColor(color->Attribute("active"));
				if (color->Attribute("pressed"))
					pressedcolor = toColor(color->Attribute("pressed"));
				break;
			}
			colornode = xml->IterateChildren("color", colornode);
		}
		return true;
	}

	std::string MenuStyle::getName()
	{
		return name;
	}

	void MenuStyle::setButtonStyle(ButtonStyle style)
	{
		buttonstyle = style;
	}
	ButtonStyle MenuStyle::getButtonStyle()
	{
		return buttonstyle;
	}

	unsigned int MenuStyle::getBackgroundColor()
	{
		return bgcolor;
	}
	unsigned int MenuStyle::getTextColor()
	{
		return textcolor;
	}
	unsigned int MenuStyle::getActiveColor()
	{
		return activecolor;
	}
	unsigned int MenuStyle::getPressedColor()
	{
		return pressedcolor;
	}

	unsigned int MenuStyle::toColor(std::string str)
	{
		if (str.size() <= 1 || str[0] != '#')
			return 0;
		unsigned char color[4];
		*(unsigned int*)color = strtoul(str.c_str() + 1, 0, 16);
		return (color[0] << 16)
			+ (color[1] << 8)
			+ color[2];
	}
}
