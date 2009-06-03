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

#ifndef _BULLET_HPP_
#define _BULLET_HPP_

#include "ReferenceCounted.hpp"
#include "Vector2.hpp"
#include "Weapon.hpp"

#include <vector>

namespace backlot
{
	/**
	 * Server-side bullet-type projectile information.
	 */
	class Bullet : public ReferenceCounted
	{
		public:
			Bullet(WeaponPointer weapon);
			~Bullet();

			void setPosition(Vector2F position);
			Vector2F getPosition();

			void setSpeed(Vector2F speed);
			Vector2F getSpeed();

			bool update();
			static void updateAll();
		private:
			WeaponPointer weapon;
			Vector2F position;
			Vector2F speed;

			static std::vector<SharedPointer<Bullet> > bullets;
	};

	typedef SharedPointer<Bullet> BulletPointer;
}

#endif