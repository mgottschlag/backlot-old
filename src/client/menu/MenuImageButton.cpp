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

#include "menu/MenuImageButton.hpp"
#include "Engine.hpp"
#include "menu/InputReceiver.hpp"
#include "support/tinyxml.h"

#include <guichan/widgets/imagebutton.hpp>

namespace backlot
{
	MenuImageButton::MenuImageButton() : MenuElement()
	{
	}
	MenuImageButton::~MenuImageButton()
	{
	}

	void MenuImageButton::load(TiXmlElement *xml, InputReceiver *input)
	{
		// Load image button
		gcn::ImageButton *image;
		if (xml->Attribute("image"))
		{
			std::string filename = Engine::get().getGameDirectory() + "/sprites/"
				+ xml->Attribute("image");
			image = new gcn::ImageButton(filename);
		}
		else
			image = new gcn::ImageButton();
		widget = image;
		if (input)
			image->addActionListener(input);
		// Load children
		MenuElement::load(xml, input);
	}

	void MenuImageButton::setTexture(TexturePointer texture)
	{
	}
	TexturePointer MenuImageButton::getTexture()
	{
	}
}
