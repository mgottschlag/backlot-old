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

#include "entity/EntityImage.hpp"
#include "entity/Entity.hpp"

namespace backlot
{
	EntityImage::EntityImage(Entity *entity) : ReferenceCounted(),
		entity(entity)
	{
	}
	EntityImage::~EntityImage()
	{
	}

	bool EntityImage::load(std::string image)
	{
		// Load the texture
		texture = new Texture(ETF_Linear);
		if (!texture->load(image))
			return false;
		return true;
	}

	void EntityImage::setPosition(Vector2F position)
	{
		this->position = position;
	}
	Vector2F EntityImage::getPosition()
	{
		return position;
	}

	void EntityImage::setSize(Vector2F size)
	{
		this->size = size;
	}
	Vector2F EntityImage::getSize()
	{
		return size;
	}

	void EntityImage::setRotation(float rotation)
	{
		this->rotation = rotation;
	}
	float EntityImage::getRotation()
	{
		return rotation;
	}

	void EntityImage::setDepth(float depth)
	{
		this->depth = depth;
	}
	float EntityImage::getDepth()
	{
		return depth;
	}

	void EntityImage::rotate(float rotation, Vector2F center)
	{
	}

	void EntityImage::render()
	{
	}
	void EntityImage::renderAll()
	{
		std::list<EntityImage*>::iterator it = images.begin();
		while (it != images.end())
		{
			(*it)->render();
			it++;
		}
	}

	std::list<EntityImage*> EntityImage::images;
}
