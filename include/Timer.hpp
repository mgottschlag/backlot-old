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

#ifndef _TIMER_HPP_
#define _TIMER_HPP_

#include "ReferenceCounted.hpp"
#include "Script.hpp"

namespace backlot
{
	class Timer : public ReferenceCounted
	{
		public:
			Timer();
			~Timer();

			void setInterval(unsigned int msecs);
			void setPeriodic(bool periodic);

			void setTime(unsigned int msecs);
			unsigned int getTime();

			void start();
			void stop();
			void reset();
			bool isRunning();

			void setCallback(ScriptPointer script, std::string function);

			static void callCallbacks();
		private:
			ScriptPointer script;
			std::string function;

			unsigned int interval;
			bool periodic;
			unsigned int time;
			uint64_t starttime;
			bool running;

			static std::vector<Timer*> timers;
	};
}

#endif
