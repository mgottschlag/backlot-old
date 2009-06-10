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

#include "Dialog.hpp"
#include "Engine.hpp"

#include "support/tinyxml.h"

#include <iostream>

namespace backlot
{
	Dialog::Dialog()
	{
		loaded = false;
		visible = false;
	}
	Dialog::~Dialog()
	{
	}

	SharedPointer<Dialog> Dialog::get(std::string name)
	{
		// Get already loaded dialog
		std::map<std::string, DialogPointer>::iterator it = dialogs.find(name);
		if (it != dialogs.end())
		{
			return it->second;
		}
		// Load dialog
		DialogPointer dialog = new Dialog();
		if (!dialog->load(name))
		{
			return 0;
		}
		return dialog;
	}

	bool Dialog::load(std::string name)
	{
		// Open XML file
		std::string filename = Engine::get().getGameDirectory() + "/dialogs/" + name + ".xml";
		TiXmlDocument xml(filename.c_str());
		if (!xml.LoadFile() || xml.Error())
		{
			std::cerr << "Could not load XML file " << name << ".xml: " << xml.ErrorDesc() << std::endl;
			return false;
		}
		TiXmlNode *node = xml.FirstChild("dialog");
		if (!node)
		{
			std::cerr << "Parser error: <dialog> not found." << std::endl;
			return false;
		}
		TiXmlElement *root = node->ToElement();
		// Load widgets
		if (!parseWidgets(root))
		{
			return false;
		}
		// Load scripts
		script = new Script();
		script->addCoreFunctions();
		script->addMenuFunctions();
		TiXmlNode *scriptnode = root->FirstChild("script");
		while (scriptnode)
		{
			TiXmlElement *scriptdata = scriptnode->ToElement();
			std::string code;
			if (scriptdata)
			{
				TiXmlNode *textnode = scriptdata->FirstChild();
				while (textnode)
				{
					TiXmlText *text = textnode->ToText();
					if (text && text->Value())
					{
						code += text->Value();
					}
					textnode = scriptdata->IterateChildren(textnode);
				}
			}
			script->runString(code);
			scriptnode = node->IterateChildren("script", scriptnode);
		}
		// Insert dialog into loaded list
		this->name = name;
		loaded = true;
		dialogs.insert(std::pair<std::string, DialogPointer>(name, this));
		return true;
	}
	void Dialog::unload()
	{
		grab();
		// Remove from loaded dialogs
		if (loaded)
		{
			std::map<std::string, DialogPointer>::iterator it = dialogs.find(name);
			if (it != dialogs.end())
			{
				dialogs.erase(it);
			}
		}
		drop();
	}

	void Dialog::unloadAll()
	{
		// Simply delete all content of the loaded list
		dialogs.clear();
	}

	void Dialog::setVisible(bool visible)
	{
		this->visible = visible;
	}
	bool Dialog::isVisible()
	{
		return visible;
	}

	bool Dialog::parseWidgets(TiXmlElement *xml)
	{
		return true;
	}

	std::map<std::string, SharedPointer<Dialog> > Dialog::dialogs;
}
