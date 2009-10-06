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

#ifndef _CLIENTMAP_HPP_
#define _CLIENTMAP_HPP_

#include "Map.hpp"
#include "graphics/QuadBatch.hpp"

namespace backlot
{
	struct MapLayer
	{
		TexturePointer texture;
		bool shadow;
		std::vector<QuadBatchPointer> batches;
	};

	class ClientMap : public Map
	{
		public:
			ClientMap();
			virtual ~ClientMap();

			/**
			 * Loads the map with the given name or returns an already loaded
			 * one.
			 */
			static SharedPointer<ClientMap> get(std::string name);

			virtual bool load(std::string name);

			/**
			 * If visible is true, the map is set as the currently drawn one,
			 * else it is hidden.
			 */
			void setVisible(bool visible);
			/**
			 * Returns true if the map is currently shown.
			 */
			bool isVisible();
			/**
			 * Returns the currently visible map.
			 */
			static SharedPointer<ClientMap> getVisibleMap();

			/**
			 * Renders the map.
			 */
			void render();
		private:
			std::vector<MapLayer> layers;
			static ClientMap *visible;

			static std::map<std::string, ClientMap*> maps;
	};

	typedef SharedPointer<ClientMap> ClientMapPointer;
}

#endif
