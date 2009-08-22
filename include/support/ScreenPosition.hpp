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

#ifndef _SCREENPOSITION_HPP_
#define _SCREENPOSITION_HPP_

#include "Vector2.hpp"

namespace backlot
{
	/**
	 * Class to hold the position on the screen for menu or overlay layouts.
	 * Positions are stored in two parts: A relative part (with 2 components
	 * going from 0 to 1 where 0/0 is the top left and 1/1 the bottom right
	 * screen corner) and an absolute part in pixels which is added onto the
	 * relative position.
	 */
	class ScreenPosition
	{
		public:
			/**
			 * Constructor.
			 */
			ScreenPosition()
			{
			}
			/**
			 * Constructor.
			 */
			ScreenPosition(const Vector2F &relative,
				const Vector2I &absolute = Vector2I(0, 0))
			{
				rel = relative;
				abs = absolute;
			}
			/**
			 * Destructor.
			 */
			~ScreenPosition()
			{
			}

			ScreenPosition operator+(const ScreenPosition &sp) const
			{
				return ScreenPosition(rel + sp.rel, abs + sp.abs);
			}
			ScreenPosition operator-(const ScreenPosition &sp) const
			{
				return ScreenPosition(rel - sp.rel, abs - sp.abs);
			}
			ScreenPosition &operator=(const ScreenPosition &sp)
			{
				rel = sp.rel;
				abs = sp.abs;
				return *this;
			}
			ScreenPosition &operator+=(const ScreenPosition &sp)
			{
				rel += sp.rel;
				abs += sp.abs;
				return *this;
			}
			ScreenPosition &operator-=(const ScreenPosition &sp)
			{
				rel -= sp.rel;
				abs -= sp.abs;
				return *this;
			}

			/**
			 * Relative part of the position (1.0 means screen width/height).
			 */
			Vector2F rel;
			/**
			 * Absolute part of the position in pixels.
			 */
			Vector2I abs;
	};
}

#endif
