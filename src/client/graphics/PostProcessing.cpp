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
#include "Preferences.hpp"

#include "support/tinyxml.h"

#include <iostream>
#include <GL/gl.h>

namespace backlot
{
	PostProcessing::PostProcessing() : ReferenceCounted(), active(true)
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
			std::cerr << "Parser error: <effect> not found." << std::endl;
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
				passes.push_back(pass);
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

	void PostProcessing::setActive(bool active)
	{
		this->active = active;
	}
	bool PostProcessing::isActive()
	{
		return active;
	}

	void PostProcessing::addPass(PostProcessingPassPointer pass)
	{
		passes.push_back(pass);
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
		// Reset camera (we are only going to render full screen quads)
		Vector2I screensize = Preferences::get().getResolution();
		glDisable(GL_BLEND);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glScalef(1.0 / screensize.x, 1.0 / screensize.y, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		// Apply post processing passes
		for (unsigned int i = 0; i < effects.size(); i++)
		{
			if (effects[i]->isActive())
				effects[i]->apply();
		}
		// Draw result
		PostProcessingPass::drawResult();
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
	}

	void PostProcessing::apply()
	{
		// Just loop through all passes
		for (unsigned int i = 0; i < passes.size(); i++)
		{
			passes[i]->draw();
		}
	}

	std::vector<PostProcessing*> PostProcessing::effects;
}
