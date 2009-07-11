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

#include "QuadList.hpp"
#include "Tile.hpp"
#include "TileSet.hpp"

#include <cstring>
#include <iostream>

QuadList::QuadList(TileSet *tileset, bool shadow) : tileset(tileset),
	shadow(shadow)
{
	vertexcount = 0;
	vertices = 0;
	texcoords = 0;
}
QuadList::~QuadList()
{
	if (vertices)
		delete[] vertices;
	if (texcoords)
		delete[] texcoords;
}

void QuadList::addQuads(const std::vector<Quad> &quads, int x, int y)
{
	for (unsigned int i = 0; i < quads.size(); i++)
		addQuad(quads[i], x, y);
}
void QuadList::addQuad(const Quad &quad, int x, int y)
{
	// Allocate more memory
	float *newvertices = new float[vertexcount * 3 + 12];
	memcpy(newvertices, vertices, vertexcount * 3 * sizeof(float));
	if (vertices)
		delete[] vertices;
	vertices = newvertices;
	newvertices = &vertices[vertexcount * 3];
	float *newtexcoords = new float[vertexcount * 2 + 8];
	memcpy(newtexcoords, texcoords, vertexcount * 2 * sizeof(float));
	if (texcoords)
		delete[] texcoords;
	texcoords = newtexcoords;
	newtexcoords = &texcoords[vertexcount * 2];
	vertexcount += 4;
	// Get quad info
	float height = quad.height;
	Vector2F offset = quad.offset;
	RectangleI texture = quad.texture;
	float texx = (float)texture.x / tileset->getTextureSize().x;
	float texy = (float)texture.y / tileset->getTextureSize().y;
	float texwidth = (float)texture.width / tileset->getTextureSize().x;
	float texheight = (float)texture.height / tileset->getTextureSize().y;
	// Fill in texture coordinates
	float tc[8];
	tc[0] = texx;
	tc[1] = texy;
	tc[2] = texx;
	tc[3] = texy + texheight;
	tc[2] = texx + texwidth;
	tc[3] = texy + texheight;
	tc[2] = texx + texwidth;
	tc[3] = texy;
	// Rotate
	for (unsigned int i = 0; i < 4; i++)
	{
		newtexcoords[i * 2] = tc[(i * 2 + 8 - quad.rotated * 2) % 8];
		newtexcoords[i * 2 + 1] = tc[(i * 2 + 8 - quad.rotated * 2) % 8 + 1];
	}
	// Fill in vertex data
	newvertices[0] = offset.x;
	newvertices[1] = offset.y;
	newvertices[3] = offset.x;
	switch (quad.rotated)
	{
		case 0:
		case 2:
			newvertices[4] = offset.y + (float)texture.height / 32;
			newvertices[6] = offset.x + (float)texture.width / 32;
			newvertices[7] = offset.y + (float)texture.height / 32;
			newvertices[9] = offset.x + (float)texture.width / 32;
			break;
		case 1:
		case 3:
			newvertices[4] = offset.y + (float)texture.width / 32;
			newvertices[6] = offset.x + (float)texture.height / 32;
			newvertices[7] = offset.y + (float)texture.width / 32;
			newvertices[9] = offset.x + (float)texture.height / 32;
			break;
	}
	newvertices[10] = offset.y;
	for (unsigned int i = 0; i < 4; i++)
	{
		newvertices[i * 3] += x;
		newvertices[i * 3 + 1] += y;
	}
	// Height
	newvertices[2] = height;
	newvertices[5] = height;
	newvertices[8] = height;
	newvertices[11] = height;
	// Set initial bounding rectangle if this is the first quad
	if (vertexcount == 4)
	{
		boundingrect.x = newvertices[0];
		boundingrect.y = newvertices[1];
	}
	// Increase bounding rectangle
	boundingrect.insertPoint(Vector2F(newvertices[0], newvertices[1]));
	boundingrect.insertPoint(Vector2F(newvertices[3], newvertices[4]));
	boundingrect.insertPoint(Vector2F(newvertices[6], newvertices[7]));
	boundingrect.insertPoint(Vector2F(newvertices[9], newvertices[10]));
}
void QuadList::addQuad(float *quadvertices, float *quadtexcoords)
{
	// Allocate more memory and copy data
	float *newvertices = new float[vertexcount * 3 + 12];
	memcpy(newvertices, vertices, vertexcount * 3 * sizeof(float));
	if (vertices)
		delete[] vertices;
	vertices = newvertices;
	newvertices = &vertices[vertexcount * 3];
	memcpy(newvertices, quadvertices, 12 * sizeof(float));
	float *newtexcoords = new float[vertexcount * 2 + 8];
	memcpy(newtexcoords, texcoords, vertexcount * 2 * sizeof(float));
	if (texcoords)
		delete[] texcoords;
	texcoords = newtexcoords;
	newtexcoords = &texcoords[vertexcount * 2];
	memcpy(newtexcoords, quadtexcoords, 8 * sizeof(float));
	vertexcount += 4;
	// Set initial bounding rectangle if this is the first quad
	if (vertexcount == 4)
	{
		boundingrect.x = newvertices[0];
		boundingrect.y = newvertices[1];
	}
	// Increase bounding rectangle
	boundingrect.insertPoint(Vector2F(newvertices[0], newvertices[1]));
	boundingrect.insertPoint(Vector2F(newvertices[3], newvertices[4]));
	boundingrect.insertPoint(Vector2F(newvertices[6], newvertices[7]));
	boundingrect.insertPoint(Vector2F(newvertices[9], newvertices[10]));
}

std::vector<QuadList*> QuadList::split()
{
	std::vector<QuadList*> lists;
	std::vector<RectangleI> listrects;
	// Go through all quads and sort them into the different lists
	for (unsigned int i = 0; i < vertexcount / 4; i++)
	{
		Vector2F firstvertex(vertices[i * 12], vertices[i * 12 + 1]);
		QuadList *list = 0;
		// Check whether there is already a list into which the quad fits
		for (unsigned int j = 0; j < lists.size(); j++)
		{
			if (listrects[j].contains(firstvertex))
			{
				list = lists[j];
				break;
			}
		}
		// Create new list if necessary
		if (!list)
		{
			list = new QuadList(tileset, shadow);
			RectangleI rect;
			rect.x = (int)firstvertex.x;
			if ((int)firstvertex.x % 64 < 0)
				rect.x -= 64 - (int)firstvertex.x % 64;
			else
				rect.x -= (int)firstvertex.x % 64;
			rect.y = (int)firstvertex.y;
			if ((int)firstvertex.y % 64 < 0)
				rect.y -= 64 - (int)firstvertex.y % 64;
			else
				rect.y -= (int)firstvertex.y % 64;
			rect.width = 64;
			rect.height = 64;
			lists.push_back(list);
			listrects.push_back(rect);
		}
		// Add quad to list
		list->addQuad(&vertices[i * 12], &texcoords[i * 8]);
	}
	std::cout << lists.size() << " quad lists generated." << std::endl;
	return lists;
}

int QuadList::getVertexCount()
{
	return vertexcount;
}
float *QuadList::getVertices()
{
	return vertices;
}
float *QuadList::getTextureCoords()
{
	return texcoords;
}

RectangleF QuadList::getBoundingRect()
{
	return boundingrect;
}
