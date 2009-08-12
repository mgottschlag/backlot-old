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

#include "menu/Menu.hpp"
#include "Preferences.hpp"
#include "Engine.hpp"
#include "Preferences.hpp"
#include "Graphics.hpp"

#include "support/tinyxml.h"

#include <GL/gl.h>
#include <iostream>
#include <guichan/widgets/button.hpp>
#include <guichan/widgets/container.hpp>
#include <guichan/actionlistener.hpp>

namespace backlot
{
	class MenuListener : public gcn::ActionListener
	{
		public:
			MenuListener(Menu *menu)
			{
				this->menu = menu;
			}

			virtual void action(const gcn::ActionEvent &actionEvent)
			{
				menu->buttonPressed(actionEvent);
			}
		private:
			Menu *menu;
	};

	Menu::Menu() : ReferenceCounted()
	{
		// Create input receiver
		listener = new MenuListener(this);
	}

	Menu::~Menu()
	{
		// Hide
		if (isActive())
			setActive(false);
		// Remove from loaded menus
		std::map<std::string, Menu*>::iterator it = menus.find(name);
		if (it != menus.end())
		{
			menus.erase(it);
		}
		// Delete listener
		delete listener;
	}

	SharedPointer<Menu> Menu::get(std::string name)
	{
		// Get already loaded menu
		std::map<std::string, Menu*>::iterator it = menus.find(name);
		if (it != menus.end())
		{
			return it->second;
		}
		// Load map
		MenuPointer menu = new Menu();
		if (!menu->load(name))
		{
			return 0;
		}
		return menu;
	}

	bool Menu::load(std::string name)
	{
		// Remove from loaded menus
		std::map<std::string, Menu*>::iterator it = menus.find(name);
		if (it != menus.end())
		{
			menus.erase(it);
		}
		// Get font
		font = Font::get("menu");
		if (!font)
			return false;
		// Open XML file
		std::string filename = Engine::get().getGameDirectory() + "/menus/" + name + ".xml";
		TiXmlDocument xml(filename.c_str());
		if (!xml.LoadFile() || xml.Error())
		{
			std::cerr << "Could not load XML file " << name << ".xml: " << xml.ErrorDesc() << std::endl;
			return false;
		}
		TiXmlNode *node = xml.FirstChild("menu");
		if (!node)
		{
			std::cerr << "Parser error: <menu> not found." << std::endl;
			return false;
		}
		TiXmlElement *root = node->ToElement();
		// Get items
		TiXmlNode *itemnode = root->FirstChild("item");
		while (itemnode)
		{
			TiXmlElement *itemdata = itemnode->ToElement();
			if (itemdata)
			{
				// Get item data
				int position = 0;
				if (itemdata->Attribute("position", &position) == 0)
				{
					std::cerr << "Missing menu item position." << std::endl;
					return false;
				}
				const char *label = itemdata->Attribute("label");
				if (!label)
				{
					std::cerr << "Missing menu item label." << std::endl;
					return false;
				}
				// Add item
				gcn::Button *button = new gcn::Button();
				button->setCaption(label);
				items.insert(items.begin() + position, button);
			}
			itemnode = node->IterateChildren("item", itemnode);
		}
		// Position widgets
		Vector2I screensize = Preferences::get().getResolution();
		int xpos = screensize.x - 200;
		int ypos = screensize.y - 40 * items.size();
		for (unsigned int i = 0; i < items.size(); i++)
		{
			items[i]->setDimension(gcn::Rectangle(xpos, ypos + i * 40, 180, 30));
			Graphics::get().getGuichanContainer()->add(items[i]);
			items[i]->addActionListener(listener);
		}
		// Load script
		script = new Script();
		script->addCoreFunctions();
		script->addClientFunctions();
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
		// Add to loaded maps
		this->name = name;
		menus.insert(std::pair<std::string, Menu*>(name, this));
		return true;
	}

	void Menu::setActive(bool active)
	{
		if (active)
		{
			if (Menu::active && Menu::active != this)
				Menu::active->setActive(false);
			Menu::active = this;
			// Show items
			for (unsigned int i = 0; i < items.size(); i++)
				items[i]->setVisible(true);
		}
		else if (Menu::active == this)
		{
			Menu::active = 0;
			// Hide items
			for (unsigned int i = 0; i < items.size(); i++)
				items[i]->setVisible(false);
		}
	}
	bool Menu::isActive()
	{
		return active == this;
	}
	SharedPointer<Menu> Menu::getActiveMenu()
	{
		return active;
	}

	void Menu::render()
	{
		// Setup camera
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		const Vector2I &size = Preferences::get().getResolution();
		glOrtho(0, size.x, size.y, 0, -1, 1);

		// Render background
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glColor3f(0.25, 0.25, 0.25);
		glBegin(GL_QUADS);
			glVertex2f(0, 0);
			glVertex2f(size.x, 0);
			glVertex2f(size.x, size.y);
			glVertex2f(0, size.y);
		glEnd();
		glColor3f(1.0, 1.0, 1.0);
		glDisable(GL_BLEND);

		font->render("Press Escape to close menu.", Vector2I(0, 0));

		glEnable(GL_DEPTH_TEST);
		// Reenable camera
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}

	void Menu::buttonPressed(const gcn::ActionEvent &event)
	{
		// Look for menu item
		for (unsigned int i = 0; i < items.size(); i++)
		{
			if (event.getSource() == items[i])
			{
				// Call event handler
				if (script->isFunction("on_menu_item"))
				{
					script->callFunction("on_menu_item", (int)i);
				}
				break;
			}
		}
	}

	Menu *Menu::active;

	std::map<std::string, Menu*> Menu::menus;
}
