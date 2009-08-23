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

#ifndef _MENUELEMENT_HPP_
#define _MENUELEMENT_HPP_

#include "ReferenceCounted.hpp"
#include "ScreenPosition.hpp"
#include "menu/MenuStyle.hpp"

#include <vector>

namespace gcn
{
	class Widget;
	class Container;
}

namespace backlot
{
	/**
	 * Base class for all menu GUI elements. Menu elements are stored in a
	 * hierarchial form in order to ensure correct drawing order at all times.
	 */
	class MenuElement : public ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 */
			MenuElement();
			/**
			 * Destructor.
			 */
			~MenuElement();

			/**
			 * Loads the element from an xml file.
			 */
			virtual void load(TiXmlElement *xml);

			/**
			 * Sets the position of the menu element.
			 */
			void setPosition(const ScreenPosition &position);
			/**
			 * Returns the position of the menu element.
			 */
			ScreenPosition getPosition();

			/**
			 * Sets the size of the menu element.
			 */
			void setSize(const ScreenPosition &size);
			/**
			 * Returns the size of the menu element.
			 */
			ScreenPosition getSize();

			/**
			 * Sets the style definition of the element.
			 */
			void setStyle(MenuStylePointer style);
			/**
			 * Returns the style definition of the element. If no style has been
			 * set, the parent elements are searched for one.
			 */
			MenuStylePointer getStyle();

			/**
			 * Callback after the style of the element has changed. On a style
			 * change, child elements without their own style are also updated.
			 */
			virtual void changedStyle();

			/**
			 * Sets the parent of the menu element.
			 */
			void setParent(SharedPointer<MenuElement> parent);
			/**
			 * Returns the parent of the element.
			 */
			SharedPointer<MenuElement> getParent();

			/**
			 * Sets whether this menu element is visible. Note that when parent
			 * elements are invisible this setting does not have any effect,
			 * instead the element stays invisible.
			 */
			void setVisible(bool visible);
			/**
			 * Returns whether this menu element is actually visible. This
			 * includes invisible parent elements.
			 */
			bool isVisible();

			/**
			 * Sets the ID of the menu element. The ID mainly is used for input
			 * handling.
			 */
			void setID(unsigned int id);
			/**
			 * Returns the ID of the element.
			 */
			unsigned int getID();
		protected:
			ScreenPosition position;
			ScreenPosition size;
			MenuStylePointer style;
			bool visible;

			std::vector<SharedPointer<MenuElement> > children;
			WeakPointer<MenuElement> parent;

			gcn::Container *container;
			gcn::Widget *widget;

			unsigned int id;
	};

	typedef SharedPointer<MenuElement> MenuElementPointer;
}

#endif
