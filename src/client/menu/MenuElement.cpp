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

#include "menu/MenuElement.hpp"
#include "menu/MenuImage.hpp"
#include "menu/MenuButton.hpp"
#include "menu/MenuImageButton.hpp"
#include "Graphics.hpp"
#include "support/tinyxml.h"

#include <guichan/widgets/container.hpp>

namespace backlot
{
	MenuElement::MenuElement() : ReferenceCounted()
	{
		id = 0;
		widget = 0;
		container = 0;
	}
	MenuElement::~MenuElement()
	{
	}

	void MenuElement::load(TiXmlElement *xml, InputReceiver *input)
	{
		// Create dummy widget if not already done
		if (!container)
			container = new gcn::Container;
		Graphics::get().getGuichanContainer()->add(container);
		container->setOpaque(false);
		if (widget)
			container->add(widget);
		// Load id
		if (xml->Attribute("id"))
		{
			id = atoi(xml->Attribute("id"));
			widget->setActionEventId(xml->Attribute("id"));
		}
		// Load position
		TiXmlNode *positionnode = xml->FirstChild("position");
		while (positionnode)
		{
			TiXmlElement *position = positionnode->ToElement();
			if (position)
			{
				ScreenPosition newpos;
				if (position->Attribute("relative"))
					newpos.rel = position->Attribute("relative");
				if (position->Attribute("absolute"))
					newpos.abs = position->Attribute("absolute");
				setPosition(newpos);
				break;
			}
			positionnode = xml->IterateChildren("position", positionnode);
		}
		// Load size
		TiXmlNode *sizenode = xml->FirstChild("size");
		while (sizenode)
		{
			TiXmlElement *size = sizenode->ToElement();
			if (size)
			{
				ScreenPosition newsize;
				if (size->Attribute("relative"))
					newsize.rel = size->Attribute("relative");
				if (size->Attribute("absolute"))
					newsize.abs = size->Attribute("absolute");
				setSize(newsize);
				break;
			}
			sizenode = xml->IterateChildren("size", sizenode);
		}
		// Load children
		TiXmlNode *widgetnode = xml->FirstChild("button");
		while (widgetnode)
		{
			TiXmlElement *widget = widgetnode->ToElement();
			if (widget)
			{
				MenuButtonPointer image = new MenuButton();
				image->load(widget, input);
				image->setParent(this);
			}
			widgetnode = xml->IterateChildren("button", widgetnode);
		}
		widgetnode = xml->FirstChild("image");
		while (widgetnode)
		{
			TiXmlElement *widget = widgetnode->ToElement();
			if (widget)
			{
				MenuImagePointer image = new MenuImage();
				image->load(widget, input);
				image->setParent(this);
			}
			widgetnode = xml->IterateChildren("image", widgetnode);
		}
		widgetnode = xml->FirstChild("imagebutton");
		while (widgetnode)
		{
			TiXmlElement *widget = widgetnode->ToElement();
			if (widget)
			{
				MenuImageButtonPointer image = new MenuImageButton();
				image->load(widget, input);
				image->setParent(this);
			}
			widgetnode = xml->IterateChildren("imagebutton", widgetnode);
		}

		changedStyle();
	}

	void MenuElement::setPosition(const ScreenPosition &position)
	{
		this->position = position;
		Vector2I pos = position.get();
		if (container)
			container->setPosition(pos.x, pos.y);
	}
	ScreenPosition MenuElement::getPosition()
	{
		return position;
	}

	void MenuElement::setSize(const ScreenPosition &size)
	{
		this->size = size;
		Vector2I pxsize = size.get();
		if (container)
			container->setSize(pxsize.x, pxsize.y);
		if (widget)
			widget->setSize(pxsize.x, pxsize.y);
	}
	ScreenPosition MenuElement::getSize()
	{
		return size;
	}

	void MenuElement::setStyle(MenuStylePointer style)
	{
		if (this->style == style)
			return;
		this->style = style;
		changedStyle();
	}
	MenuStylePointer MenuElement::getStyle()
	{
		if (!style && parent)
			return parent->getStyle();
		return style;
	}

	void MenuElement::changedStyle()
	{
		for (unsigned int i = 0; i < children.size(); i++)
		{
			if (!children[i]->style)
				children[i]->changedStyle();
		}
	}

	void MenuElement::setParent(SharedPointer<MenuElement> parent)
	{
		if (this->parent.get() == parent.get())
			return;
		// Add widget to parent container
		if (parent)
		{
			parent->container->add(container);
		}
		else
		{
			Graphics::get().getGuichanContainer()->add(container);
		}
		// Remove element from the old parent
		if (this->parent)
		{
			for (unsigned int i = 0; i < this->parent->children.size(); i++)
			{
				if (this->parent->children[i] == this)
				{
					this->parent->children.erase(this->parent->children.begin() + i);
					break;
				}
			}
		}
		// Set parent
		this->parent = parent;
		// Add to parent list
		if (parent)
		{
			parent->children.push_back(this);
		}
	}
	SharedPointer<MenuElement> MenuElement::getParent()
	{
		return parent.get();
	}

	void MenuElement::setVisible(bool visible)
	{
		if (this->visible == visible)
			return;
		this->visible = visible;
		// Change visibility of this element
		container->setVisible(visible);
		// Change visibility of all children
		// TODO
	}
	bool MenuElement::isVisible()
	{
		if (!visible)
			return false;
		// Check visibility recursively
		if (parent && !parent->isVisible())
			return false;
		return true;
	}

	void MenuElement::setID(unsigned int id)
	{
		this->id = id;
	}
	unsigned int MenuElement::getID()
	{
		return id;
	}
}
