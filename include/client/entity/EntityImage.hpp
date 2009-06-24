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

#ifndef _ENTITYIMAGE_HPP_
#define _ENTITYIMAGE_HPP_

#include "ReferenceCounted.hpp"
#include "Texture.hpp"
#include "Vector2.hpp"

#include <list>

namespace backlot
{
	class Entity;

	class EntityImage : public ReferenceCounted
	{
		public:
			EntityImage(Entity *entity);
			~EntityImage();

			bool load(std::string image);
			void setTexture(TexturePointer texture);

			void setPosition(Vector2F position);
			Vector2F getPosition();

			void setSize(Vector2F size);
			Vector2F getSize();

			void setRotation(float rotation);
			float getRotation();

			void setDepth(float depth);
			float getDepth();

			void setVisible(bool visible);
			bool isVisible();

			void rotate(float rotation, Vector2F center);

			void render();
			static void renderAll();
		private:
			Entity *entity;
			TexturePointer texture;

			Vector2F position;
			Vector2F size;
			float rotation;
			float depth;
			bool visible;

			static std::list<EntityImage*> images;
	};

	typedef SharedPointer<EntityImage> EntityImagePointer;
}

#endif
