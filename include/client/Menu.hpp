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

#ifndef _MENU_HPP_
#define _MENU_HPP_

#include "ReferenceCounted.hpp"
#include "Font.hpp"

#include <string>
#include <map>
#include <vector>

namespace gcn
{
	class Button;
}

namespace backlot
{
	/**
	 * Client menu.
	 */
	class Menu : public ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 */
			Menu();
			/**
			 * Destructor.
			 */
			~Menu();

			/**
			 * Loads the menu with the given name or returns an already loaded
			 * one.
			 */
			static SharedPointer<Menu> get(std::string name);

			/**
			 * Loads a menu.
			 */
			bool load(std::string name);

			/**
			 * Sets whether the menu is shown at the moment. If set to active,
			 * the menu decatives any other menu which is active at this time.
			 */
			void setActive(bool active);
			/**
			 * Returns whether the menu is shown right now.
			 */
			bool isActive();
			/**
			 * Returns the currently active menu.
			 */
			static SharedPointer<Menu> getActiveMenu();

			/**
			 * Renders this menu.
			 */
			void render();
		private:
			std::string name;

			FontPointer font;

			std::vector<gcn::Button*> items;

			static Menu *active;
			static std::map<std::string, Menu*> menus;
	};

	typedef SharedPointer<Menu> MenuPointer;
}

#endif
