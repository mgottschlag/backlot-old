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

#ifndef _ANIMATION_HPP_
#define _ANIMATION_HPP_

#include "Vector2.hpp"
#include "ReferenceCounted.hpp"

#include <vector>

namespace backlot
{
	class Animation : public ReferenceCounted
	{
		public:
			Animation();
			~Animation();

			/**
			 * Set the number of the single frames in a texture. The total number of frames are x*y.
			 * @param x The number fo single frames in x direction.
			 * @param y The number of single frames in y direction.
			 */
			void setFrameNumbers(int x, int y);

			/**
			 * Set the time which the animation needs to be played one time.
			 */
			void setPeriod(float period);
			
			/**
			 * Set how often the animation is played. -1 for infinite loop.
			 */
			void setLoop(int loopcount);

			/**
			 * If the animation is currently played.
			 */
			bool isPlaying();
			
			/**
			 * Draw one single frame.
			 * @param frame The number of the frame to draw.
			 */
			void draw(int frame);
			/**
			 * Draw the next frame of the animation.
			 */
			bool draw();
			
			void start();
			void stop();
		private:
			Vector2I frames;
			Vector2F sizes;

			unsigned int starttime;
			float frametime;

			int loopcount;
			bool isplayed;

			static std::vector<Animation*> animations;
	};

	typedef SharedPointer<Animation> AnimationPointer;
}

#endif
