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

#include "Font.hpp"
#include "Engine.hpp"

#include <iostream>
#include <fstream>
#include <GL/gl.h>

namespace backlot
{
	Font::Font()
	{
	}
	Font::~Font()
	{
		// Remove from loaded fonts
		std::map<std::string, Font*>::iterator it = fonts.find(name);
		if (it != fonts.end())
		{
			fonts.erase(it);
		}
	}

	SharedPointer<Font> Font::get(std::string name)
	{
		// Get already loaded font
		std::map<std::string, Font*>::iterator it = fonts.find(name);
		if (it != fonts.end())
		{
			return it->second;
		}
		// Load map
		FontPointer font = new Font();
		if (!font->load(name))
		{
			return 0;
		}
		return font;
	}

	bool Font::load(std::string name)
	{
		// Open texture
		texture = new Texture(ETF_Linear);
		if (!texture->load(std::string("fonts/") + name + ".png"))
		{
			std::cerr << "Could not load font texture." << std::endl;
			texture = 0;
			return false;
		}
		// Load glyph rectangles
		std::ifstream glyphs((Engine::get().getGameDirectory() + "/fonts/"
			+ name + ".font").c_str(),
			std::ios_base::in | std::ios_base::binary);
		if (!glyphs)
		{
			std::cerr << "Could not load glyph info." << std::endl;
			texture = 0;
			return false;
		}
		glyphs.read((char*)glyphinfo, 256 * 4);
		glyphs.close();
		// Add to loaded fonts
		this->name = name;
		fonts.insert(std::pair<std::string, Font*>(name, this));
		return true;
	}

	Vector2I Font::getSize(std::string text)
	{
		int width = 0;
		int height = 0;
		for (unsigned int i = 0; i < text.size(); i++)
		{
			width += glyphinfo[(unsigned char)text[i]][2];
			if (height < glyphinfo[(unsigned char)text[i]][3])
				height = glyphinfo[(unsigned char)text[i]][3];
		}
		return Vector2I(width, height);
	}
	void Font::render(std::string text, Vector2F position, Vector2F size,
		unsigned int color)
	{
		Vector2I pixelsize = getSize(text);
		glEnable(GL_TEXTURE_2D);
		texture->bind();
		glColor3f(1.0, 0.0, 0.0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_QUADS);
		Vector2F scale(size.x / pixelsize.x, size.y / pixelsize.y);
		for (unsigned int i = 0; i < text.size(); i++)
		{
			Vector2F coord1((float)glyphinfo[(unsigned char)text[i]][0] / 256,
				(float)glyphinfo[(unsigned char)text[i]][1] / 256);
			Vector2F coord2((float)glyphinfo[(unsigned char)text[i]][0] / 256
				+ (float)glyphinfo[(unsigned char)text[i]][2] / 256,
				(float)glyphinfo[(unsigned char)text[i]][1] / 256
				+ (float)glyphinfo[(unsigned char)text[i]][3] / 256);
			float width = scale.x * glyphinfo[(unsigned char)text[i]][2];
			float height = scale.y * glyphinfo[(unsigned char)text[i]][2];
			glTexCoord2f(coord1.x, coord1.y);
			glVertex2f(position.x, position.y);
			glTexCoord2f(coord2.x, coord1.y);
			glVertex2f(position.x + width, position.y);
			glTexCoord2f(coord2.x, coord2.y);
			glVertex2f(position.x + width, position.y + height);
			glTexCoord2f(coord1.x, coord2.y);
			glVertex2f(position.x, position.y + height);
			position.x += width;
		}
		glEnd();
		glDisable(GL_BLEND);
		glColor3f(1.0, 1.0, 1.0);
		glDisable(GL_TEXTURE_2D);
	}

	std::map<std::string, Font*> Font::fonts;
}
