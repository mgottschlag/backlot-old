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

#include "QuadBatch.hpp"

#include <GL/glew.h>

namespace backlot
{
	QuadBatch::QuadBatch()
	{
		dirty = false;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ibo);
	}
	QuadBatch::~QuadBatch()
	{
	}

	void QuadBatch::addQuad(const Quad &quad)
	{
		quads.push_back(quad);
		setDirty(true);
	}
	void QuadBatch::removeQuad(int index)
	{
		quads.erase(quads.begin() + index);
		setDirty(true);
	}
	std::vector<Quad> &QuadBatch::getQuads()
	{
		return quads;
	}

	unsigned int QuadBatch::getSize()
	{
		return quads.size();
	}

	void QuadBatch::setDirty(bool dirty)
	{
		this->dirty = dirty;
	}
	bool QuadBatch::isDirty()
	{
		return dirty;
	}
	void QuadBatch::rebuild()
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		// Compact vertex data
		float *vertexdata = new float[getSize() * 4 * sizeof(float) * 5];
		// Positions
		for (unsigned int i = 0; i < getSize(); i++)
		{
			Vector2F position = quads[i].position;
			Vector2F size = quads[i].size;
			float z = quads[i].depth;

			vertexdata[i * 12] = position.x;
			vertexdata[i * 12 + 1] = position.y;
			vertexdata[i * 12 + 2] = z;

			vertexdata[i * 12 + 3] = position.x + size.x;
			vertexdata[i * 12 + 3 + 1] = position.y;
			vertexdata[i * 12 + 3 + 2] = z;

			vertexdata[i * 12 + 6] = position.x + size.x;
			vertexdata[i * 12 + 6 + 1] = position.y + size.y;
			vertexdata[i * 12 + 6 + 2] = z;

			vertexdata[i * 12 + 9] = position.x;
			vertexdata[i * 12 + 9 + 1] = position.y + size.y;
			vertexdata[i * 12 + 9 + 2] = z;
		}
		// Texture coordinates
		for (unsigned int i = 0; i < getSize(); i++)
		{
			RectangleF rect = quads[i].texturecoords;
			vertexdata[getSize() * 12 + i * 8] = rect.x;
			vertexdata[getSize() * 12 + i * 8 + 1] = rect.y;

			vertexdata[getSize() * 12 + i * 8 + 2] = rect.x + rect.width;
			vertexdata[getSize() * 12 + i * 8 + 2 + 1] = rect.y;

			vertexdata[getSize() * 12 + i * 8 + 4] = rect.x + rect.width;
			vertexdata[getSize() * 12 + i * 8 + 4 + 1] = rect.y + rect.height;

			vertexdata[getSize() * 12 + i * 8 + 6] = rect.x;
			vertexdata[getSize() * 12 + i * 8 + 6 + 1] = rect.y + rect.height;
		}
		// Create VBO
		glBufferDataARB(GL_ARRAY_BUFFER, getSize() * 4 * sizeof(float) * 5, 0, GL_STATIC_DRAW);
		delete[] vertexdata;
		// Index buffer data
		if (getSize() * 4 <= 256)
		{
			unsigned char *indexdata = new unsigned char[getSize() * 4];
			for (unsigned int i = 0; i < getSize() * 4; i++)
				indexdata[i] = i;
			indexsize = 1;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, getSize() * 4, indexdata, GL_STATIC_DRAW);
			delete[] indexdata;
		}
		else if (getSize() * 4 <= 65536)
		{
			unsigned short *indexdata = new unsigned short[getSize() * 4];
			for (unsigned int i = 0; i < getSize() * 4; i++)
				indexdata[i] = i;
			indexsize = 2;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, getSize() * 8, indexdata, GL_STATIC_DRAW);
			delete[] indexdata;
		}
		else
		{
			unsigned int *indexdata = new unsigned int[getSize() * 4];
			for (unsigned int i = 0; i < getSize() * 4; i++)
				indexdata[i] = i;
			indexsize = 4;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, getSize() * 16, indexdata, GL_STATIC_DRAW);
			delete[] indexdata;
		}
		// Batch data is up-to-date
		dirty = false;
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void QuadBatch::render()
	{
		// Set up arrays
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);
		glTexCoordPointer(2, GL_FLOAT, 0, (char*)(getSize() * 12));
		// Draw
		if (indexsize == 1)
			glDrawElements(GL_QUADS, getSize() * 4, GL_UNSIGNED_BYTE, 0);
		else if (indexsize == 2)
			glDrawElements(GL_QUADS, getSize() * 4, GL_UNSIGNED_SHORT, 0);
		else
			glDrawElements(GL_QUADS, getSize() * 4, GL_UNSIGNED_INT, 0);
		// Clean up again
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
