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

#include "menu/SimpleButton.hpp"

#include <guichan/font.hpp>

namespace backlot
{
	SimpleButton::SimpleButton(const std::string &caption) : gcn::Button(caption)
	{
		normalcolor = 0x000000;
		activecolor = 0x333333;
		pressedcolor = 0x222222;
		setFrameSize(0);
	}

	void SimpleButton::draw(gcn::Graphics *graphics)
	{
		// Get text position
		int y = getHeight() / 2 - getFont()->getHeight() / 2;
		int x;
		switch (getAlignment())
		{
			case gcn::Graphics::LEFT:
				x = mSpacing;
				break;
			case gcn::Graphics::CENTER:
				x = getWidth() / 2;
				break;
			case gcn::Graphics::RIGHT:
				x = getWidth() - mSpacing;
				break;
			default:
				x = mSpacing;
		}
		// Draw
		graphics->setFont(getFont());
		if (isPressed())
		{
			graphics->setColor(pressedcolor);
			graphics->drawText(getCaption(), x + 1, y + 1, getAlignment());
		}
		else
		{
			if (mHasMouse)
				graphics->setColor(activecolor);
			else
				graphics->setColor(normalcolor);
			graphics->drawText(getCaption(), x, y, getAlignment());
		}
	}

	void SimpleButton::setColor(unsigned int normal, unsigned int active,
		unsigned int pressed)
	{
		normalcolor = normal;
		activecolor = active;
		pressedcolor = pressed;
	}
}
