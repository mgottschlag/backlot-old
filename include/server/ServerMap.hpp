/*
Copyright (c) 2009, Mathias Gottschlag

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef _SERVERMAP_HPP_
#define _SERVERMAP_HPP_

#include "Map.hpp"
#include "entity/EntityState.hpp"

namespace backlot
{
	class ServerMap : public Map
	{
		public:
			ServerMap();
			virtual ~ServerMap();

			/**
			 * Loads the map with the given name or returns an already loaded
			 * one.
			 */
			static SharedPointer<ServerMap> get(std::string name);

			virtual bool load(std::string name);

			/**
			 * Adds the initial entities to the game.
			 */
			void loadEntities();
		private:
			std::list<EntityStatePointer> entities;

			static std::map<std::string, ServerMap*> maps;
	};

	typedef SharedPointer<ServerMap> ServerMapPointer;
}

#endif
