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

#include "graphics/QuadBatch.hpp"

#include <GL/glew.h>
#include <iostream>

namespace backlot
{
	QuadBatch::QuadBatch() : ReferenceCounted()
	{
		vertexdata = 0;
		quadcount = 0;
		if (GLEW_ARB_vertex_buffer_object)
			glGenBuffersARB(1, &vbo);
	}
	QuadBatch::QuadBatch(unsigned int quadcount, float *vertexdata)
	{
		this->vertexdata = 0;
		this->quadcount = 0;
		if (GLEW_ARB_vertex_buffer_object)
			glGenBuffersARB(1, &vbo);
		set(quadcount, vertexdata);
	}
	QuadBatch::~QuadBatch()
	{
		if (GLEW_ARB_vertex_buffer_object)
			glDeleteBuffersARB(1, &vbo);
		if (vertexdata)
			delete[] vertexdata;
	}

	void QuadBatch::set(unsigned int quadcount, float *vertexdata)
	{
		// Delete old data
		if (this->vertexdata)
		{
			delete[] this->vertexdata;
			this->vertexdata = 0;
		}
		this->vertexdata = vertexdata;
		this->quadcount = quadcount;
		// Create VBO
		if (GLEW_ARB_vertex_buffer_object)
		{
			glBindBufferARB(GL_ARRAY_BUFFER, vbo);
			glBufferDataARB(GL_ARRAY_BUFFER, getSize() * 4 * sizeof(float) * 5, vertexdata, GL_STATIC_DRAW);
			glBindBufferARB(GL_ARRAY_BUFFER, 0);
			// We don't need the data any more
			delete[] this->vertexdata;
			this->vertexdata = 0;
		}
	}

	unsigned int QuadBatch::getSize()
	{
		return quadcount;
	}

	void QuadBatch::render()
	{
		if (GLEW_ARB_vertex_buffer_object)
		{
			// Set up arrays
			glBindBufferARB(GL_ARRAY_BUFFER, vbo);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, 0);
			glTexCoordPointer(2, GL_FLOAT, 0, (char*)(getSize() * 48));
			// Draw
			glDrawArrays(GL_QUADS, 0, getSize() * 4);
			// Clean up again
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			glBindBufferARB(GL_ARRAY_BUFFER, 0);
		}
		else
		{
			// Set up arrays
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, vertexdata);
			glTexCoordPointer(2, GL_FLOAT, 0, (char*)vertexdata + getSize() * 48);
			// Draw
			glDrawArrays(GL_QUADS, 0, getSize() * 4);
			// Clean up again
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
		}
	}
}
