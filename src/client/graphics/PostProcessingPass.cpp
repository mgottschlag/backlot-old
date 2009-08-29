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
#include <GL/glew.h>

namespace backlot
{
	PostProcessingPass::PostProcessingPass() : ReferenceCounted()
	{
		psversion = ESV_None;
		vsversion = ESV_None;
	}
	PostProcessingPass::~PostProcessingPass()
	{
	}

	bool PostProcessingPass::load(TiXmlElement *xml)
	{
		ShaderVersion psversion = ESV_None;
		std::string pscode;
		ShaderVersion vsversion = ESV_None;
		std::string vscode;
		// Load pixel shader
		TiXmlNode *psnode = xml->FirstChild("pixelshader");
		while (psnode)
		{
			TiXmlElement *psdata = psnode->ToElement();
			if (psdata)
			{
				// Get shader code
				std::string code;
				TiXmlNode *textnode = psdata->FirstChild();
				while (textnode)
				{
					TiXmlText *text = textnode->ToText();
					if (text && text->Value())
					{
						code += text->Value();
					}
					textnode = psdata->IterateChildren(textnode);
				}
				while (isspace(code[0]))
					code = code.substr(1);
				// Get version
				ShaderVersion version = ESV_None;
				if (psdata->Attribute("type"))
				{
					if (!strcmp(psdata->Attribute("type"), "arbfp1.0")
						&& GLEW_ARB_fragment_program)
						version = ESV_ARBFP10;
				}
				// Set shader to the best supported version
				if (version > psversion)
				{
					psversion = version;
					pscode = code;
				}
			}
			psnode = xml->IterateChildren("pixelshader", psnode);
		}
		if (psversion == ESV_None)
		{
			std::cerr << "No usable shader!" << std::endl;
			return false;
		}
		else if (psversion == ESV_ARBFP10)
		{
			// Build shader
			this->psversion = psversion;
			glGenProgramsARB(1, &ps);
			glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, ps);
			glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,
				GL_PROGRAM_FORMAT_ASCII_ARB, pscode.size(),
				pscode.c_str());
			unsigned int error = glGetError();
			if (error == GL_INVALID_OPERATION)
			{
				std::cout << glGetString(GL_PROGRAM_ERROR_STRING_ARB) << std::endl;
				std::cout << "Code: \"" << pscode << "\"" << std::endl;
				return false;
			}
		}
		else
		{
			std::cerr << "Unimplemented shader version!" << std::endl;
			return false;
		}
		// Load vertex shader
		TiXmlNode *vsnode = xml->FirstChild("vertexshader");
		while (vsnode)
		{
			TiXmlElement *vsdata = vsnode->ToElement();
			if (vsdata)
			{
				// Get shader code
				std::string code;
				TiXmlNode *textnode = vsdata->FirstChild();
				while (textnode)
				{
					TiXmlText *text = textnode->ToText();
					if (text && text->Value())
					{
						code += text->Value();
					}
					textnode = vsdata->IterateChildren(textnode);
				}
				while (isspace(code[0]))
					code = code.substr(1);
				// Get version
				ShaderVersion version = ESV_None;
				if (vsdata->Attribute("type"))
				{
					if (!strcmp(vsdata->Attribute("type"), "arbvp1.0")
						&& GLEW_ARB_vertex_program)
						version = ESV_ARBVP10;
				}
				// Set shader to the best supported version
				if (version > vsversion)
				{
					vsversion = version;
					vscode = code;
				}
			}
			vsnode = xml->IterateChildren("vertexshader", vsnode);
		}
		if (vsversion == ESV_None)
		{
			std::cerr << "No usable shader!" << std::endl;
			return false;
		}
		else if (vsversion == ESV_ARBVP10)
		{
			// Build shader
			this->vsversion = vsversion;
			glGenProgramsARB(1, &vs);
			glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vs);
			glProgramStringARB(GL_VERTEX_PROGRAM_ARB,
				GL_PROGRAM_FORMAT_ASCII_ARB, vscode.size(),
				vscode.c_str());
			unsigned int error = glGetError();
			if (error == GL_INVALID_OPERATION)
			{
				std::cout << glGetString(GL_PROGRAM_ERROR_STRING_ARB) << std::endl;
				std::cout << "Code: \"" << vscode << "\"" << std::endl;
				return false;
			}
		}
		else
		{
			std::cerr << "Unimplemented shader version!" << std::endl;
			return false;
		}
		return true;
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
		current->bind();
		drawFSQuad();
	}

	void PostProcessingPass::draw()
	{
		std::cout << "Program: " << ps << std::endl;
		// Draw fullscreen quad into the next render buffer
		rtt->start(next);
		current->bind();
		if (vsversion == ESV_ARBVP10)
		{
			glEnable(GL_VERTEX_PROGRAM_ARB);
			glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vs);
		}
		if (psversion == ESV_ARBFP10)
		{
			glEnable(GL_FRAGMENT_PROGRAM_ARB);
			glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, ps);
		}
		unsigned int error = glGetError();
		if (error != GL_NO_ERROR)
			std::cout << "Pass (1): " << gluErrorString(error) << std::endl;
		drawFSQuad();
		if (psversion == ESV_ARBFP10)
		{
			glDisable(GL_FRAGMENT_PROGRAM_ARB);
		}
		if (vsversion == ESV_ARBVP10)
		{
			glDisable(GL_VERTEX_PROGRAM_ARB);
		}
		rtt->end();
		// Swap render targets
		TexturePointer tmp = current;
		current = next;
		next = tmp;
	}

	void PostProcessingPass::drawFSQuad()
	{
		Vector2I screensize = Preferences::get().getResolution();
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_QUADS);
			glTexCoord2f(screensize.x, 0.0);
			glVertex2f(1, -1);
			glTexCoord2f(0.0, 0.0);
			glVertex2f(-1, -1);
			glTexCoord2f(0.0, screensize.y);
			glVertex2f(-1, 1);
			glTexCoord2f(screensize.x, screensize.y);
			glVertex2f(1, 1);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	TexturePointer PostProcessingPass::current;
	TexturePointer PostProcessingPass::next;
	RenderTargetPointer PostProcessingPass::rtt;
}
