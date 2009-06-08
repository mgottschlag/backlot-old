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
#include "tinyxml.h"

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

		TiXmlNode *root;
		TiXmlNode *node;
		TiXmlElement *element;

		if ((root = configfile.FirstChild("hud")) == NULL)
		{
			std::cerr << "Could not find \"hud\" in \"" << configfilepath << "\"\n";
			return false;
		}
		if ((node = root->FirstChild("element")) == NULL)
		{
			std::cerr << "Could not find \"health\" in \"hud\" in \"" << configfilepath << "\"\n";
			return false;
		}

		for (int i = 0; node; i++)
		{
			hudelements.push_back(HUDElement());
			hudelements[i].load(node->ToElement());
			node = root->IterateChildren("element", node);
		}

		return true;
	}

	void HUD::render()
	{
		for (unsigned int i = 0; i < hudelements.size(); i++)
			hudelements[i].render();
	}

	HUD::HUD()
	{
	}
}
