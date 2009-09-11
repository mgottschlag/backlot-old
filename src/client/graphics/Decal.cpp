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

#include "graphics/Decal.hpp"

#include <GL/gl.h>

namespace backlot
{
	bool Decal::add(std::string texture, Vector2F position, Vector2F size,
		float depth)
	{
		// Create decal
		Decal *decal = new Decal(position, size, depth);
		if (!decal->load(texture))
		{
			delete decal;
			return false;
		}
		// Add to list
		decals.push_back(decal);
		// Limit decal count
		if (decals.size() > 200)
		{
			delete decals.front();
			decals.pop_front();
		}
		return true;
	}

	void Decal::renderAll()
	{
		// Setup material
		// TODO: Disable Z writes?
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Draw decals
		std::deque<Decal*>::iterator it = decals.begin();
		while (it != decals.end())
		{
			(*it)->render();
			it++;
		}
		// Reset material
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}

	Decal::Decal(Vector2F position, Vector2F size, float depth)
		: position(position), size(size), depth(depth)
	{
	}
	Decal::~Decal()
	{
	}

	bool Decal::load(std::string texture)
	{
		this->texture = Texture::get(texture);
		if (!this->texture)
			return false;
		return true;
	}

	void Decal::render()
	{
		glPushMatrix();
		glTranslatef(position.x, position.y, 10.0);
		texture->bind();
		glBegin(GL_QUADS);
			glTexCoord2f(0, 1);
			glVertex3f(-size.x / 2, -size.y / 2, 0);
			glTexCoord2f(1, 1);
			glVertex3f(size.x / 2, -size.y / 2, 0);
			glTexCoord2f(1, 0);
			glVertex3f(size.x / 2, size.y / 2, 0);
			glTexCoord2f(0, 0);
			glVertex3f(-size.x / 2, size.y / 2, 0);
		glEnd();
		glPopMatrix();
	}

	std::deque<Decal*> Decal::decals;
}
