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

#include "menu/MenuButton.hpp"
#include "menu/SimpleButton.hpp"
#include "support/tinyxml.h"

#include <guichan/widgets/button.hpp>
#include <guichan/widgets/container.hpp>

namespace backlot
{
	MenuButton::MenuButton() : MenuElement()
	{
		buttonstyle = EBS_Normal;
	}
	MenuButton::~MenuButton()
	{
	}

	void MenuButton::load(TiXmlElement *xml)
	{
		// Load button
		gcn::Button *button = new gcn::Button();
		widget = button;
		if (xml->Attribute("label"))
			button->setCaption(xml->Attribute("label"));
		// Load children
		MenuElement::load(xml);
	}

	void MenuButton::changedStyle()
	{
		// Check whether button has to be recreated
		MenuStylePointer style = getStyle();
		if (style && buttonstyle != style->getButtonStyle())
		{
			std::string caption = ((gcn::Button*)widget)->getCaption();
			container->remove(widget);
			delete widget;
			buttonstyle = style->getButtonStyle();
			if (buttonstyle == EBS_Normal)
			{
				widget = new gcn::Button(caption);
			}
			else if (buttonstyle == EBS_Simple)
			{
				widget = new SimpleButton(caption);
			}
			container->add(widget);
		}
		// Set style
		if (buttonstyle == EBS_Simple)
		{
			((SimpleButton*)widget)->setColor(style->getTextColor(),
				style->getActiveColor(), style->getPressedColor());
		}
		// Update children
		MenuElement::changedStyle();
	}
}
