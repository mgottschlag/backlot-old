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

#include "Tile.hpp"
#include "TileSet.hpp"

#include "tinyxml.h"

#include <iostream>
#include <GL/gl.h>

Tile::Tile(TileSet *tileset) : tileset(tileset)
{
}
Tile::~Tile()
{
}

bool Tile::load(TiXmlElement *xml)
{
	// Read attributes
	if (!xml->Attribute("name"))
	{
		std::cerr << "Tile: No name given." << std::endl;
		return false;
	}
	name = xml->Attribute("name");
	if (!xml->Attribute("size") || !xml->Attribute("height"))
	{
		std::cerr << name << ": size or height missing." << std::endl;
		return false;
	}
	size = xml->Attribute("size");
	size /= 32;
	height = atof(xml->Attribute("height"));
	// Load quads
	TiXmlNode *quadnode = xml->FirstChild("quad");
	while (quadnode)
	{
		TiXmlElement *quad = quadnode->ToElement();
		if (!quad)
		{
			quadnode = xml->IterateChildren("quad", quadnode);
			continue;
		}
		// Quad attributes
		if (!quad->Attribute("height") || !quad->Attribute("texture"))
		{
			std::cerr << "Missing quad height or texture." << std::endl;
			return false;
		}
		float height = atof(quad->Attribute("height"));
		RectangleI texturerect = quad->Attribute("texture");

		Vector2I offset;
		if (quad->Attribute("offset"))
		{
			offset = quad->Attribute("offset");
		}
		offset /= 32;
		int rotated = 0;
		if (quad->Attribute("rotated"))
		{
			rotated = atoi(quad->Attribute("rotated"));
		}
		bool shadow = false;
		if (quad->Attribute("type"))
		{
			shadow = !strcmp(quad->Attribute("type"), "shadow");
		}
		// Add quad to tile
		Quad quadinfo;
		quadinfo.height = height;
		quadinfo.texture = texturerect;
		quadinfo.offset = offset;
		quadinfo.rotated = rotated;
		if (shadow)
			shadowquads.push_back(quadinfo);
		else
			quads.push_back(quadinfo);
		quadnode = xml->IterateChildren("quad", quadnode);
	}
	std::cout << tileset->getName() << "." << name << " loaded." << std::endl;
	return true;
}

std::string Tile::getName()
{
	return name;
}
TileSet *Tile::getTileSet()
{
	return tileset;
}

const Vector2F &Tile::getSize()
{
	return size;
}
float Tile::getHeight()
{
	return height;
}

const std::vector<Quad> &Tile::getQuads()
{
	return quads;
}
const std::vector<Quad> &Tile::getShadowQuads()
{
	return shadowquads;
}

void Tile::render(int x, int y)
{
	glPushMatrix();
	glTranslatef(x, y, 0);
	glBindTexture(GL_TEXTURE_2D, tileset->getTexture());
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glBegin(GL_QUADS);
	for (unsigned int i = 0; i < quads.size(); i++)
	{
		drawQuad(quads[i]);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
void Tile::renderShadows(int x, int y)
{
	glPushMatrix();
	glTranslatef(x, y, 0);
	glBindTexture(GL_TEXTURE_2D, tileset->getTexture());
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_ZERO, GL_SRC_COLOR);
	glEnable(GL_BLEND);
	glColor4f(1.0, 1.0, 1.0, 1.0);
		glBegin(GL_QUADS);
	for (unsigned int i = 0; i < shadowquads.size(); i++)
	{
		drawQuad(shadowquads[i]);
	}
	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void Tile::drawQuad(Quad &quad)
{
	float height = quad.height;
	Vector2F offset = quad.offset;
	RectangleI texture = quad.texture;
	float texx = (float)texture.x / tileset->getTextureSize().x;
	float texy = (float)texture.y / tileset->getTextureSize().y;
	float texwidth = (float)texture.width / tileset->getTextureSize().x;
	float texheight = (float)texture.height / tileset->getTextureSize().y;
	switch (quad.rotated)
	{
		case 0:
			glTexCoord2f(texx, texy);
			glVertex3f(offset.x, offset.y, height);
			glTexCoord2f(texx, texy + texheight);
			glVertex3f(offset.x, offset.y + (float)texture.height / 32, height);
			glTexCoord2f(texx + texwidth, texy + texheight);
			glVertex3f(offset.x + (float)texture.width / 32, offset.y + (float)texture.height / 32, height);
			glTexCoord2f(texx + texwidth, texy);
			glVertex3f(offset.x + (float)texture.width / 32, offset.y, height);
			break;
		case 1:
			glTexCoord2f(texx, texy + texheight);
			glVertex3f(offset.x, offset.y, height);
			glTexCoord2f(texx + texwidth, texy + texheight);
			glVertex3f(offset.x, offset.y + (float)texture.width / 32, height);
			glTexCoord2f(texx + texwidth, texy);
			glVertex3f(offset.x + (float)texture.height / 32, offset.y + (float)texture.width / 32, height);
			glTexCoord2f(texx, texy);
			glVertex3f(offset.x + (float)texture.height / 32, offset.y, height);
			break;
		case 2:
			glTexCoord2f(texx + texwidth, texy + texheight);
			glVertex3f(offset.x, offset.y, height);
			glTexCoord2f(texx + texwidth, texy);
			glVertex3f(offset.x, offset.y + (float)texture.height / 32, height);
			glTexCoord2f(texx, texy);
			glVertex3f(offset.x + (float)texture.width / 32, offset.y + (float)texture.height / 32, height);
			glTexCoord2f(texx, texy + texheight);
			glVertex3f(offset.x + (float)texture.width / 32, offset.y, height);
			break;
		case 3:
			glTexCoord2f(texx + texwidth, texy);
			glVertex3f(offset.x, offset.y, height);
			glTexCoord2f(texx, texy);
			glVertex3f(offset.x, offset.y + (float)texture.width / 32, height);
			glTexCoord2f(texx, texy + texheight);
			glVertex3f(offset.x + (float)texture.height / 32, offset.y + (float)texture.width / 32, height);
			glTexCoord2f(texx + texwidth, texy + texheight);
			glVertex3f(offset.x + (float)texture.height / 32, offset.y, height);
			break;
	}
}
