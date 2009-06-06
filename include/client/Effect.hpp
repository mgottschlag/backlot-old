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

#ifndef _EFFECT_HPP_
#define _EFFECT_HPP_

#include "Sound.hpp"
#include "Texture.hpp"
#include "Animation.hpp"
#include "Vector2.hpp"
#include "ReferenceCounted.hpp"

#include <string>

namespace backlot
{
	class Effect : public ReferenceCounted
	{
		public:
			Effect();
			~Effect();

			/**
			 * Loads the animation and the sound.
			 * @param texturepath The texture file used for the animation.
			 * @param framenumber The number of frames in x an y direction of the texture.
			 * @param soundpath The sound file that is used for effect sound.
			 * @return If everything was loaded correctly return true.
			 */
			bool load(std::string texturepath, Vector2I framenuber, std::string soundpath);

			/**
			 * Set the periode of the animation.
			 */
			void setPeriod(float time);

			/**
			 * Renders the animation.
			 */
			void render();
			/**
			 * Start animation and sound playback.
			 * @param loop How often the effect is played. -1 for infinite loop.
			 */
			void play(int loop = -1);
			/**
			 * Stop the animation and sound playback.
			 */
			void stop();
			
		private:
			TexturePointer texture;
			AnimationPointer animation;
			SoundPointer sound;

			bool playing;
	};

	typedef SharedPointer<Effect> EffectPointer;
}

#endif
