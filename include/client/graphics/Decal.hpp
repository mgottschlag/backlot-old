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

#ifndef _DECAL_HPP_
#define _DECAL_HPP_

#include "Vector2.hpp"
#include "graphics/Texture.hpp"

#include <deque>

namespace backlot
{
	/**
	 * Class for transparent map overlays. Only a limited number of decals can
	 * be created at once (currently 200), after that the oldest decals are
	 * deleted. Decals are drawn with simple alpha channel transparency.
	 */
	class Decal
	{
		public:
			/**
			 * Creates a new decal.
			 * @param texture Path to the texture (relative to the game
			 * directory).
			 * @param position Center of the new decal.
			 */
			static bool add(std::string texture, Vector2F position,
				Vector2F size, float depth);

			/**
			 * Renders all decals.
			 */
			static void renderAll();

			~Decal();
		private:
			Decal(Vector2F position, Vector2F size, float depth);

			bool load(std::string texture);

			void render();

			Vector2F position;
			Vector2F size;
			float depth;
			TexturePointer texture;

			static std::deque<Decal*> decals;
	};
}

#endif
