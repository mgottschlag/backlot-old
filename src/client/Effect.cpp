/*
Copyright (C) 2009  Simon Kerler, Mathias Gottschlag

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
#include <iostream>

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

	bool Effect::load(TexturePointer texture, Vector2I framenumber, SoundPointer sound)
	{
		this->texture = texture;
		animation->setFrameNumbers(framenumber.x, framenumber.y);
		this->sound = sound;
		return true;
	}

	void Effect::setPeriod(float time)
	{
		animation->setPeriod(time);
	}

	bool Effect::render()
	{
		if (!playing)
			return false;
		glPushMatrix();
		//glScalef(5.0, 5.0, 5.0);
		texture->bind();
		animation->draw();
		glPopMatrix();
		if (!animation->isPlaying() && (!sound || !sound->isPlaying()))
			return false;
		return true;
	}

	void Effect::play(int loop)
	{
		animation->setLoop(loop);
		animation->start();
		if (sound)
			sound->play(loop);
		playing = true;
		// Add to effect list
		effects.push_back(this);
	}
	void Effect::stop()
	{
		if (playing == true)
		{
			// Stop effect
			animation->stop();
			if (sound)
				sound->stop();
			playing = false;
		}
		// Remove from effect list
		for (unsigned int i = 0; i < effects.size(); i++)
		{
			if (effects[i] == this)
			{
				std::cout << "Erased " << i << std::endl;
				effects.erase(effects.begin() + i);
				break;
			}
		}
	}

	void Effect::renderAll()
	{
		for (int i = 0; i < (int)effects.size(); i++)
		{
			if (!effects[i]->render())
			{
				std::cout << "Deleting effect " << i << "." << std::endl;
				effects[i]->stop();
				i--;
			}
		}
	}

	std::vector<EffectPointer> Effect::effects;
}
