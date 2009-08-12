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

#ifndef _GUICHANFONT_HPP_
#define _GUICHANFONT_HPP_

#include "Font.hpp"

#include <guichan/font.hpp>

namespace backlot
{
	/**
	 * Wrapper class for a guichan font using the Font class.
	 */
	class GuichanFont : public gcn::Font
	{
		public:
			GuichanFont(FontPointer font);
			virtual ~GuichanFont();

			virtual int getWidth(const std::string& text) const;
			virtual int getHeight() const;
			virtual void drawString(gcn::Graphics *graphics,
				const std::string& text, int x, int y);
		private:
			FontPointer font;
	};
}

#endif
