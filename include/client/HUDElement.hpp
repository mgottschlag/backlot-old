/*
Copyright (C) 2009  Simon Kerler

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

#ifndef _HUDELEMENT_HPP_
#define _HUDELEMENT_HPP_

#include "Vector2.hpp"
#include "Font.hpp"
#include "tinyxml.h"

namespace backlot
{
	enum HUDElementTypes
	{
		EHET_Health,
		EHET_Ammo
	};

	class HUDElement
	{
		public:
			/**
			 * Constructur.
			 */
			HUDElement();
			~HUDElement();

			/**
			 * Initialize the element.
			 * @param xmlnode Takes a pointer to a xml node (from a config file, see also
			 * HUD.hpp) to initialise its member variables.
			 */
			bool load(TiXmlNode *xmlnode);

			void render();
		private:
			int type;
			Vector2F position;
			Vector2F offset;
			Vector2F size;
			FontPointer font;
			Vector2F fontoffset;
			int fontsize;
	};
}
#endif
