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

#include "overlay/OverlayImage.hpp"
#include "Preferences.hpp"

#include <iostream>
#include <GL/gl.h>

namespace backlot
{
		OverlayImage::OverlayImage()
		{
			texture = new Texture;
		}
		OverlayImage::~OverlayImage()
		{
		}
		
		void OverlayImage::setScale(Vector2F scale)
		{
			texturescale = scale;
		}
		Vector2F OverlayImage::getScale()
		{
			return texturescale;
		}
		void OverlayImage::setRotation(float rotation)
		{
			texturerotation = rotation;
		}
		float OverlayImage::getRotation()
		{
			return texturerotation;
		}
		void OverlayImage::setTextureOffset(Vector2I offset)
		{
			textureoffset = offset;
		}
		Vector2I OverlayImage::getTextureOffset()
		{
			return textureoffset;
		}
		
		bool OverlayImage::loadImage(std::string imagepath, Vector2I size)
		{
			if(!texture->load(imagepath))
			{
				std::cerr << "Failed to load texture for OverlayImage from " << imagepath << std::endl;
				return false;
			}
			texturesize = size;
			return true;
		}
		
		void OverlayImage::render()
		{
			Vector2I resolution = Preferences::get().getResolution();
			
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glDisable(GL_DEPTH_TEST);
			glOrtho(0, resolution.x, resolution.y, 0, -1, 1);
			// Move to the right position
			glTranslatef((position.x * resolution.x), (position.y * resolution.y), 0);
			glTranslatef(offset.x, offset.y, 0);
			// Enable texture
			texture->bind();
			glEnable(GL_TEXTURE_2D);
			// Scale and rotate texture
			glMatrixMode(GL_TEXTURE);
			glRotatef(texturerotation, 1, 0, 0);
			glScalef(texturescale.x, texturescale.y, 1);
			glTranslatef((float)(texturesize.x / textureoffset.x), (float)(texturesize.y / textureoffset.y), 0);
			// Draw texture
			glMatrixMode(GL_PROJECTION);
			glEnable(GL_QUADS);
				glTexCoord2f(0, 0);
				glVertex2f(0, 0);
				glTexCoord2f(0, imagesize.y);
				glVertex2f(0, size.y);
				glTexCoord2f(imagesize.x, imagesize.y);
				glVertex2f(size.x, size.y);
				glTexCoord2f(imagesize.x, 0);
				glVertex2f(size.x, 0);
			glDisable(GL_QUADS);
			glDisable(GL_TEXTURE_2D);
		}
}
