/*
Copyright (C) 2009  Simon Kerler

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

#include "Animation.hpp"

#include <SDL/SDL.h>
#include <GL/gl.h>

namespace backlot
{
	Animation::Animation() : ReferenceCounted()
	{
		starttime = 0;
		isplayed = false;
		frametime = 0.05;

 		animations.push_back(this);
	}
	Animation::~Animation()
	{
	}

	void Animation::setFrameNumbers(int x, int y)
	{
		// Set the number of frames.
		frames.x = x;
		frames.y = y;
		// Set the UV-Size of one frame.
		sizes.x = (float) 1/x;
		sizes.y = (float) 1/y;
	}

	void Animation::setPeriod(float period)
	{
		frametime = (float)period / (frames.x * frames.y);
	}

	void Animation::setLoop(int loopcount)
	{
		this->loopcount = loopcount;
	}
	
	bool Animation::isPlaying()
	{
		if(isplayed == false)
			return false;

		int currentframe = (float)(SDL_GetTicks() - starttime) / frametime;

		if (loopcount != -1 && currentframe >= loopcount * frames.x * frames.y)
			return false;

		return true;
	}

	void Animation::draw(int frame)
	{
		frame = frame % (frames.x * frames.y);

		glEnable(GL_TEXTURE_2D);
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glTranslatef(sizes.x * (frame % frames.x), sizes.y * (frame / frames.y), 0);

		glMatrixMode(GL_MODELVIEW);
		glBegin(GL_QUADS);
			glTexCoord2f(0, sizes.y);
			glVertex3f(-1, -1, 0);
			glTexCoord2f(sizes.x, sizes.y);
			glVertex3f(1, -1, 0);
			glTexCoord2f(sizes.x, 0);
			glVertex3f(1, 1, 0);
			glTexCoord2f(0, 0);
			glVertex3f(-1, 1, 0);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
	}
	bool Animation::draw()
	{
		if (isPlaying() == true)
		{
			// Draw the current frame
			draw((float)(SDL_GetTicks() - starttime) / frametime);
			return true;
		}
		else
			return false;
	}
	
	void Animation::start()
	{
		isplayed = true;
		starttime = SDL_GetTicks();
	}
	void Animation::stop()
	{
		isplayed = false;
		starttime = 0;
	}

	std::vector<Animation*> Animation::animations;
}
