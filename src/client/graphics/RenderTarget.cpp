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

#include "graphics/RenderTarget.hpp"

#include <GL/glew.h>

namespace backlot
{
	RenderTarget::RenderTarget(unsigned int width, unsigned int height)
		: ReferenceCounted(), width(width), height(height)
	{
		if (GLEW_EXT_framebuffer_object)
		{
			// Create frame buffer object
			glGenFramebuffersEXT(1, &fbo);
			// Create and attach depth buffer
			glGenRenderbuffersEXT(1, &depthbuffer);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT,
				width, height);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
				GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthbuffer);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
		else
		{
			fbo = 0;
			depthbuffer = 0;
		}
	}
	RenderTarget::~RenderTarget()
	{
		if (GLEW_EXT_framebuffer_object)
		{
			glDeleteFramebuffersEXT(1, &fbo);
			glDeleteRenderbuffersEXT(1, &depthbuffer);
		}
	}

	void RenderTarget::start(TexturePointer texture)
	{
		if (GLEW_EXT_framebuffer_object)
		{
			// Bind framebuffer object and attach texture
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
				GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture->getHandle(),
				0);
		}
		else
		{
			this->texture = texture;
		}
		// Set viewport
		glPushAttrib(GL_VIEWPORT_BIT);
		glViewport(0, 0, width, height);
	}
	void RenderTarget::end()
	{
		// Reset viewport
		glPopAttrib();
		if (GLEW_EXT_framebuffer_object)
		{
			// Disable the framebuffer object
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
		else
		{
			// Copy
			texture->bind();
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, width, height, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
			texture = 0;
		}
	}
}
