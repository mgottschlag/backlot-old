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

#include "graphics/PostProcessing.hpp"
#include "Engine.hpp"

#include "support/tinyxml.h"

#include <iostream>

namespace backlot
{
	PostProcessing::PostProcessing() : ReferenceCounted()
	{
	}
	PostProcessing::~PostProcessing()
	{
		// Delete from effect list
		for (unsigned int i = 0; i < effects.size(); i++)
		{
			if (effects[i] == this)
			{
				effects.erase(effects.begin() + i);
				break;
			}
		}
	}

	SharedPointer<PostProcessing> PostProcessing::get(std::string name)
	{
		// Search for existing effect
		for (unsigned int i = 0; i < effects.size(); i++)
		{
			if (effects[i]->name == name)
				return effects[i];
		}
		// Load new effect
		PostProcessingPointer effect = new PostProcessing();
		if (!effect->load(name))
			return 0;
		add(effect, name);
		return effect;
	}
	void PostProcessing::add(SharedPointer<PostProcessing> effect,
		std::string name)
	{
		effect->setName(name);
		effects.push_back(effect.get());
	}
	bool PostProcessing::load(std::string name)
	{
		// Open XML file
		std::string filename = Engine::get().getGameDirectory() + "/ppeffects/" + name + ".xml";
		TiXmlDocument xml(filename.c_str());
		if (!xml.LoadFile() || xml.Error())
		{
			std::cerr << "Could not load XML file " << name << ".xml: " << xml.ErrorDesc() << std::endl;
			return false;
		}
		TiXmlNode *node = xml.FirstChild("effect");
		if (!node)
		{
			std::cerr << "Parser error: <menu> not found." << std::endl;
			return false;
		}
		TiXmlElement *root = node->ToElement();
		// Load passes
		TiXmlNode *passnode = root->FirstChild("pass");
		while (passnode)
		{
			TiXmlElement *passdata = passnode->ToElement();
			if (passdata)
			{
				PostProcessingPassPointer pass = new PostProcessingPass();
				if (!pass->load(passdata))
					return false;
			}
			passnode = root->IterateChildren("pass", passnode);
		}
		return true;
	}

	void PostProcessing::setName(std::string name)
	{
		this->name = name;
	}
	std::string PostProcessing::getName()
	{
		return name;
	}

	void PostProcessing::init()
	{
		PostProcessingPass::init();
	}
	void PostProcessing::begin()
	{
		PostProcessingPass::beginFrame();
	}
	void PostProcessing::end()
	{
		// Finish rendering into the texture
		PostProcessingPass::endFrame();
		// Apply post processing passes
		// TODO
		// Draw result
		PostProcessingPass::drawResult();
	}

	std::vector<PostProcessing*> PostProcessing::effects;
}
