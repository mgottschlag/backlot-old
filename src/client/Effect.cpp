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

#include "Effect.hpp"

#include <GL/gl.h>

namespace backlot
{
	Effect::Effect()
	{
		texture = new Texture();
		animation = new Animation();
		sound = new Sound();
		playing = false;
	}
	Effect::~Effect()
	{
	}

	bool Effect::load(std::string texturepath, Vector2I framenumber, std::string soundpath)
	{
		if (texture->load(texturepath) == false)
			return false;
		animation->setFrameNumbers(framenumber.x, framenumber.y);
		if (sound->load(soundpath) == false)
			return false;
		return true;
	}

	void Effect::setPeriod(float time)
	{
		animation->setPeriod(time);
	}

	void Effect::render()
	{
		animation->draw();
	}

	void Effect::play(int loop)
	{
		animation->setLoop(loop);
		animation->start();
		sound->play(loop);
		playing = true;
	}
	void Effect::stop()
	{
		if (playing == true)
		{
			animation->stop();
			sound->stop();
		}
	}
}
