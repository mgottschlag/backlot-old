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

#include "Menu.hpp"

#include <GL/gl.h>

namespace backlot
{
	Menu::Menu() : ReferenceCounted()
	{
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
		// Add to loaded maps
		this->name = name;
		menus.insert(std::pair<std::string, Menu*>(name, this));
		return true;
	}

	void Menu::setActive(bool active)
	{
		if (active)
			Menu::active = this;
		else if (Menu::active == this)
			Menu::active = 0;
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

		// Render background
		glEnable(GL_BLEND);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glColor3f(0.25, 0.25, 0.25);
		glBegin(GL_QUADS);
			glVertex2f(-1.0, -1.0);
			glVertex2f(-1.0, 1.0);
			glVertex2f(1.0, 1.0);
			glVertex2f(1.0, -1.0);
		glEnd();
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		// Reenable camera
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}

	Menu *Menu::active;

	std::map<std::string, Menu*> Menu::menus;
}
