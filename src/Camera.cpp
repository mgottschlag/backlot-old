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

#include "Camera.hpp"
#include "Graphics.hpp"
#include "Preferences.hpp"

#include <GL/gl.h>

namespace backlot
{
	Camera::Camera()
	{
	}
	Camera::~Camera()
	{
	}

	void Camera::setPosition(Vector2F position)
	{
		this->position = position;
	}
	Vector2F Camera::getPosition()
	{
		return position;
	}

	void Camera::apply()
	{
		// Set orthogonal projection
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		Vector2I windowsize = Preferences::get().getResolution();
		glOrtho(-(float)windowsize.x / 32 / 2, (float)windowsize.x / 32 / 2,
			-(float)windowsize.y / 32 / 2, (float)windowsize.y / 32 / 2,
			256, -256);
		glScalef(1.0, -1.0, 1.0);
		glTranslatef(position.x, position.y, 0);
		glMatrixMode(GL_MODELVIEW);
	}
}
