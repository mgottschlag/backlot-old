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

#ifndef _MENUIMAGE_HPP_
#define _MENUIMAGE_HPP_

#include "menu/MenuElement.hpp"
#include "graphics/Texture.hpp"

namespace backlot
{
	/**
	 * Static image.
	 */
	class MenuImage : public MenuElement
	{
		public:
			MenuImage();
			~MenuImage();

			virtual void load(TiXmlElement *xml, InputReceiver *input);

			/**
			 * Sets the texture to be displayed.
			 */
			void setTexture(TexturePointer texture);
			/**
			 * Returns the texture.
			 */
			TexturePointer getTexture();
		private:
	};

	typedef SharedPointer<MenuImage> MenuImagePointer;
}

#endif
