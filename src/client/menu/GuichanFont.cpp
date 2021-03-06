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

#include "menu/GuichanFont.hpp"

#include <iostream>
#include <guichan/graphics.hpp>
#include <guichan/color.hpp>
#include <GL/gl.h>

namespace backlot
{
	GuichanFont::GuichanFont(FontPointer font) : font(font)
	{
	}
	GuichanFont::~GuichanFont()
	{
	}

	int GuichanFont::getWidth(const std::string& text) const
	{
		return font->getSize(text).x;
	}
	int GuichanFont::getHeight() const
	{
		return font->getSize("Jj").y;
	}
	void GuichanFont::drawString(gcn::Graphics *graphics,
		const std::string& text, int x, int y)
	{
		const gcn::ClipRectangle &top = graphics->getCurrentClipArea();
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		font->renderRaw(text, Vector2I(x + top.xOffset, y + top.yOffset));
		glDisable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		gcn::Color color = graphics->getColor();
		if (color.a == 255)
			glDisable(GL_BLEND);
	}
}
