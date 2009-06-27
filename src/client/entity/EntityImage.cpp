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

#include <GL/gl.h>
#include <iostream>

namespace backlot
{
	EntityImage::EntityImage(Entity *entity) : ReferenceCounted(),
		entity(entity)
	{
		visible = true;
		images.push_back(this);
	}
	EntityImage::~EntityImage()
	{
		std::list<EntityImage*>::iterator it = images.begin();
		while (it != images.end())
		{
			if (*it == this)
			{
				images.erase(it);
				break;
			}
			it++;
		}
	}

	bool EntityImage::load(std::string image)
	{
		// Load the texture
		texture = new Texture(ETF_Linear);
		if (!texture->load(image))
			return false;
		return true;
	}
	void EntityImage::setTexture(TexturePointer texture)
	{
		this->texture = texture;
	}

	void EntityImage::setPosition(Vector2F position)
	{
		this->position = position;
	}
	void EntityImage::setPosition(float x, float y)
	{
		position = Vector2F(x, y);
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

	void EntityImage::setVisible(bool visible)
	{
		this->visible = visible;
	}
	bool EntityImage::isVisible()
	{
		return visible;
	}

	void EntityImage::rotate(float rotation, Vector2F center)
	{
	}
	void EntityImage::rotate(float rotation, float centerx, float centery)
	{
		rotate(rotation, Vector2F(centerx, centery));
	}

	void EntityImage::render()
	{
		// Check whether the image can be rendered
		if (!texture || !visible)
			return;
		// Render the image
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(position.x, position.y, depth);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		if (texture)
			texture->bind();
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0);
			glVertex2f(0, 0);
			glTexCoord2f(1.0, 0.0);
			glVertex2f(size.x, 0);
			glTexCoord2f(1.0, 1.0);
			glVertex2f(size.x, size.y);
			glTexCoord2f(0.0, 1.0);
			glVertex2f(0, size.y);
		glEnd();
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glPopMatrix();
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
