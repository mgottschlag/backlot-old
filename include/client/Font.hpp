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

#ifndef _FONT_HPP_
#define _FONT_HPP_

#include "ReferenceCounted.hpp"
#include "Vector2.hpp"
#include "Texture.hpp"

#include <map>

namespace backlot
{
	/**
	 * Bitmap font class. A font is defined through:
	 * @li A 256x256 pixel large PNG texture in the fonts directory
	 * (@c fonts/\<name\>.png) which contains glyph images for all chars from 0
	 * to 255.
	 * @li A 1024 bytes large file in the same directory with the same name but
	 * the extension @c .font. It contains 4 bytes for each glyph in the font,
	 * the first two define the position, the last two the size of the glyph.
	 */
	class Font : public ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 */
			Font();
			/**
			 * Destructor.
			 */
			~Font();

			/**
			 * Loads the font with the given name or returns an already loaded
			 * one.
			 */
			static SharedPointer<Font> get(std::string name);

			/**
			 * Loads a font.
			 */
			bool load(std::string name);

			/**
			 * Returns the size (in pixels) of the text if it was rendered with
			 * this font.
			 */
			Vector2I getSize(std::string text) const;
			/**
			 * Renders the text and scales it to fit into the rectangle.
			 */
			void render(std::string text, Vector2F position, Vector2F size,
				unsigned int color = 0xFFFFFF) const;
			/**
			 * Renders the text with the font scaled to the given character height.
			 */
			void render(std::string text, Vector2F position, int height,
				unsigned int color = 0xFFFFFF) const;
			/**
			 * Renders the text.
			 */
			void render(std::string text, Vector2I position,
				unsigned int color = 0xFFFFFF) const;
			/**
			 * Renders the text without any material settings changed.
			 */
			void renderRaw(std::string text, Vector2I position) const;
		private:
			/**
			 * Name of the font.
			 */
			std::string name;
			/**
			 * Font texture.
			 */
			TexturePointer texture;
			/**
			 * Array with the glyph information.
			 */
			unsigned char glyphinfo[256][4];
			/**
			 * Loaded fonts.
			 */
			static std::map<std::string, Font*> fonts;
	};

	typedef SharedPointer<Font> FontPointer;
}

#endif
