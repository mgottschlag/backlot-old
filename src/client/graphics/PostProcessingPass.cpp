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

#include "graphics/PostProcessingPass.hpp"
#include "Preferences.hpp"

#include "support/tinyxml.h"

#include <iostream>
#include <GL/gl.h>

namespace backlot
{
	PostProcessingPass::PostProcessingPass() : ReferenceCounted()
	{
	}
	PostProcessingPass::~PostProcessingPass()
	{
	}

	bool PostProcessingPass::load(TiXmlElement *xml)
	{
		return false;
	}

	void PostProcessingPass::init()
	{
		Vector2I screensize = Preferences::get().getResolution();
		// Create two screen-sized textures
		// TODO: This might not always be used, but needs much vram
		current = Texture::create(screensize.x, screensize.y);
		next = Texture::create(screensize.x, screensize.y);
		// Create render target
		rtt = new RenderTarget(screensize.x, screensize.y);
	}
	void PostProcessingPass::beginFrame()
	{
		// Start rendering into the texture
		rtt->start(current);
	}
	void PostProcessingPass::endFrame()
	{
		// Stop rendering into the texture
		rtt->end();
	}
	void PostProcessingPass::drawResult()
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		current->bind();
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_QUADS);
			glTexCoord2f(1.0, 0.0);
			glVertex2f(1, -1);
			glTexCoord2f(0.0, 0.0);
			glVertex2f(-1, -1);
			glTexCoord2f(0.0, 1.0);
			glVertex2f(-1, 1);
			glTexCoord2f(1.0, 1.0);
			glVertex2f(1, 1);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, 0);

		glPopMatrix();
	}

	TexturePointer PostProcessingPass::current;
	TexturePointer PostProcessingPass::next;
	RenderTargetPointer PostProcessingPass::rtt;
}
