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

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include "ReferenceCounted.hpp"
#include "Vector2.hpp"
#include "Texture.hpp"
#include "Weapon.hpp"

#include <vector>

namespace backlot
{
	/**
	 * Client-side player information.
	 */
	class Player : public ReferenceCounted
	{
		public:
			Player();
			~Player();

			bool load();

			void setID(int id);
			int getID();

			void setPosition(Vector2F position);
			Vector2F getPosition();

			void setRotation(float rotation);
			float getRotation();

			void setVisible(bool visible);
			bool isVisible();

			void setLocal(bool local);
			bool isLocal();
			static SharedPointer<Player> getLocalPlayer();

			void sendKeys(uint8_t keys);
			void sendRotation(float rotation);

			void render();
			static void renderAll();
		private:
			int id;
			Vector2F position;
			float rotation;
			bool visible;

			TexturePointer texture;

			static std::vector<Player*> players;

			static Player *local;
	};

	typedef SharedPointer<Player> PlayerPointer;
}

#endif
