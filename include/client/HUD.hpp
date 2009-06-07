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

#ifndef _HUD_HPP_
#define _HUD_HPP_

#include "Font.hpp"
#include "Vector2.hpp"

namespace backlot
{
	struct HUDElement
	{
		Vector2F position;
		Vector2F offset;
		Vector2F size;
		FontPointer font;
	};

	class HUD
	{
		public:
			static HUD &get();
			~HUD();

			bool load();
			void render();
		private:
			/**
			 * Constructor.
			 */
			HUD();

			struct HUDElement healthelement;
	};
}

#endif
