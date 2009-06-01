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
		// Load config file
		setPath(Engine::get().getGameDirectory() + "/config.xml");
		std::cout << "Loading preferences from \"" << path << "\"...\n";
		TiXmlDocument conffile(path.c_str());
		if (!conffile.LoadFile())
		{
			std::cerr << "Unable to load config file \"" << path << "\"!\n";
			return false;
		}

		// Get the config child
		TiXmlNode *node;
		TiXmlElement *element;
		node = conffile.FirstChild("config");
		if (node == NULL)
		{
			std::cerr << "Can't find \"config\" in config file " << path << "!\n";
			return false;
		}
		
		// Load the preferences for the graphics
		node = node->FirstChild("graphics");
		if (node == NULL)
		{
			std::cerr << "Can't find \"graphics\" in config file " << path << "!\n";
			return false;
		}
		element = node->ToElement();

		if (element->Attribute("width", &screenresolution.x) == NULL)
		{
			std::cerr << "Can't find \"width\" in config file " << path << "!\n";
			return false;
		}
		if (element->Attribute("height", &screenresolution.y) == NULL)
		{
			std::cerr << "Can't find \"height\" in config file " << path << "!\n";
			return false;
		}
		if (element->Attribute("colordepth", &colordepth) == NULL)
		{
			std::cerr << "Can't find \"colordepth\" in config file " << path << "!\n";
			return false;
		}
		if ( ((std::string)"yes").compare(element->Attribute("fullscreen")) == 0)
			fullscreen = true;
		else if (((std::string)"no").compare(element->Attribute("fullscreen")) == 0)
			fullscreen = false;
		else
		{
			std::cerr << "Error on loading preferences: can't read vaule of attribute \"fullscreen\" in " << path << std::endl;
			return false;
		}

		// Load the preferences for the sound
		node = conffile.FirstChild("config");
		node = node->FirstChild("sound");
		if (node == NULL)
		{
			std::cerr << "Can't find \"sound\" in " << path << std::endl;
			return false;
		}
		element = node->ToElement();
		if ( ((std::string)"yes").compare(element->Attribute("stereo")) == 0)
			stereo = true;
		else if ( ((std::string)"no").compare(element->Attribute("stereo")) == 0)
			stereo = false;
		else
		{
			std::cerr << "Error on loading preferences: Can't read value of attribute \"stereo\" in " << path << std::endl;
			return false;
		}
		if (element->Attribute("frequency", &frequency) == NULL)
		{
			std::cerr << "Can't find Attribute \"frequency\" in " << path << std::endl;
			return false;
		}
		if (element->Attribute("bitrate", &bitrate) == NULL)
		{
			std::cerr << "Can't find Attribute \"bitrate\"n in " << path << std::endl;
			return false;
		}
		return true;
	}
	bool Preferences::save()
	{
		std::cout << "Saving preferences to \"" << path << "\"...\n";
		TiXmlDocument doc;
		TiXmlDeclaration *declaration = new TiXmlDeclaration("1.0", "UTF-8", "yes");
		TiXmlElement *config = new TiXmlElement("config");
		TiXmlElement *graphics = new TiXmlElement("graphics");
		TiXmlElement *sound = new TiXmlElement("sound");
		
		graphics->SetAttribute("width", screenresolution.x);
		graphics->SetAttribute("height", screenresolution.y);
		graphics->SetAttribute("colordepth", colordepth);
		if (fullscreen == true)
			graphics->SetAttribute("fullscreen", "yes");
		else
			graphics->SetAttribute("fullscreen", "no");

		if (stereo == true)
			sound->SetAttribute("stereo", "yes");
		else
			sound->SetAttribute("stereo", "no");
		sound->SetAttribute("frequency", frequency);
		sound->SetAttribute("bitrate", bitrate);
		
		doc.LinkEndChild(declaration);
		doc.LinkEndChild(config);
		config->LinkEndChild(graphics);
		config->LinkEndChild(sound);
		
		doc.SaveFile(path.c_str());

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

	bool Preferences::getStereo() const
	{
		return stereo;
	}
	int Preferences::getFrequency() const
	{
		return frequency;
	}
	int Preferences::getBitrate() const
	{
		return bitrate;
	}
	Preferences::Preferences()
	{
	}
}
