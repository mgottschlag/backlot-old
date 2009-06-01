#include "Animation.hpp"

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <iostream>

namespace backlot
{
	Animation::Animation() : ReferenceCounted()
	{
		position.x = 0;
		position.y = 0;

		starttime = 0;

		isplayed = false;

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

	void Animation::setPeriod(unsigned int period)
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

		int currentframe = (SDL_GetTicks() - starttime) / frametime;

		if (loopcount != -1 && currentframe > loopcount * frames.x * frames.y)
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
	}
	bool Animation::draw()
	{
		if (isPlaying() == true)
		{
			// Draw the current frame
			draw((SDL_GetTicks() - starttime) / frametime);
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
