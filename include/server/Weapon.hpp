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

#include <map>
#include <string>
#include <stdint.h>

namespace backlot
{
	/**
	 * Server-side weapon information.
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
			static SharedPointer<Weapon> get(std::string name);

			/**
			 * Loads the weapon info with the given name
			 */
			bool load(std::string name);

			unsigned int getID()
			{
				return id;
			}

			int getShotsPerMinute()
			{
				return rate;
			}

			int getMagazineSize()
			{
				return magazinesize;
			}
			int getMagazineCount()
			{
				return magazines;
			}
			float getBulletSpeed()
			{
				return projspeed;
			}
		private:
			std::string name;

			int rate;
			float deviation;
			int magazines;
			int magazinesize;
			float reloadtime;

			int projtype;
			float projspeed;
			int hitdamage;
			bool explosion;
			float explosionradius;
			int explosiondamage;

			unsigned int id;
			static unsigned int lastweaponid;

			static std::map<std::string, Weapon*> weapons;
	};

	typedef SharedPointer<Weapon> WeaponPointer;

	struct WeaponState
	{
		WeaponPointer weapon;
		int currentmagazine;
		int reserve;
		uint64_t lastshot;
	};
}

#endif
