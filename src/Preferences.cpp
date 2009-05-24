/*
Copyright (C) 2009   Simon Kerler

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

#include "Preferences.hpp"
#include "Engine.hpp"
#include "tinyxml.h"

#include <iostream>

namespace backlot
{
	Preferences &Preferences::get()
	{
		static Preferences preferences;
		return preferences;
	}
	
	Preferences::~Preferences()
	{
	 	// Save the preferences before deleting
	 	save();
	}
	
	bool Preferences::load()
	{
		TiXmlDocument conffile(path.c_str());
		if(!conffile.LoadFile())
		{
			std::cerr << "Unable to load config file \"" << path << "\"!\n";
			return false;
		}

		TiXmlNode *node = conffile.FirstChild("config")->FirstChild("graphics");
		TiXmlElement *element = node->ToElement();
		element->Attribute("width", &screenresolution.x);
		element->Attribute("height", &screenresolution.y);
		element->Attribute("colordepth", &colordepth);
		if (((std::string)"yes").compare(element->Attribute("fullscreen")) == 0)
			fullscreen = true;
		else if (((std::string)"no").compare(element->Attribute("fullscreen")) == 0)
			fullscreen = false;
		else
		{
			std::cerr << "Error on loading preferences: can't read vaule of attribute \"fullscreen\" in " << path << std::endl;
			return false;
		}

		std::cout << "fullscreen: " << fullscreen << std::endl;

		return true;
	}
	bool Preferences::save()
	{
		return true;
	}

	void Preferences::setPath(std::string newpath)
	{
		path = newpath;
	}
	const Vector2I &Preferences::getResolution() const
	{
		return screenresolution;
	}
	int Preferences::getColorDepth() const
	{
		return colordepth;
	}
	bool Preferences::getFullscreen() const
	{
		return fullscreen;
	}

	Preferences::Preferences()
	{
		setPath(Engine::get().getGameDirectory() + "/game/config.xml");
	}
}
