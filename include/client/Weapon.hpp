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

#ifndef _WEAPON_HPP_
#define _WEAPON_HPP_

#include "ReferenceCounted.hpp"
#include "Texture.hpp"

#include <map>
#include <string>

namespace backlot
{
	/**
	 * Client-side weapon information.
	 */
	class Weapon : public ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 */
			Weapon();
			/**
			 * Destructor.
			 */
			~Weapon();

			/**
			 * Loads the weapon with the given name or returns an already loaded
			 * one.
			 */
			static SharedPointer<Weapon> get(std::string name, int id = -1);

			/**
			 * Returns the already loaded weapon with the given id or 0 if none
			 * exists with this id.
			 */
			static SharedPointer<Weapon> get(int id);

			/**
			 * Loads the weapon info with the given name
			 */
			bool load(std::string name);

			void setID(int id);
			int getID();

			int getMagazineSize()
			{
				return magazinesize;
			}
			int getMagazineCount()
			{
				return magazines;
			}
			TexturePointer getPlayerTexture()
			{
				return playertexture;
			}
			TexturePointer getTexture()
			{
				return texture;
			}
			float getBulletSize()
			{
				return projsize;
			}
			TexturePointer getBulletTexture()
			{
				return bullettexture;
			}
		private:
			std::string name;

			TexturePointer playertexture;
			TexturePointer texture;
			TexturePointer bullettexture;

			int rate;
			float deviation;
			int magazines;
			int magazinesize;
			float reloadtime;

			int projtype;
			float projspeed;
			float projsize;
			int hitdamage;
			bool explosion;
			float explosionradius;
			int explosiondamage;

			int id;

			static std::map<std::string, Weapon*> weapons;
	};

	typedef SharedPointer<Weapon> WeaponPointer;

	struct WeaponState
	{
		WeaponPointer weapon;
		int currentmagazine;
		int reserve;
	};
}

#endif
