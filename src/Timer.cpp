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

#include "Timer.hpp"
#include "Engine.hpp"

#include <iostream>

namespace backlot
{
	Timer::Timer() : ReferenceCounted()
	{
		time = 0;
		starttime = 0;
		running = false;
		interval = 1000;
		periodic = false;
	}
	Timer::~Timer()
	{
		stop();
	}

	void Timer::setInterval(unsigned int msecs)
	{
		this->interval = msecs;
	}
	void Timer::setPeriodic(bool periodic)
	{
		this->periodic = periodic;
	}

	void Timer::setTime(unsigned int msecs)
	{
		this->time = msecs;
		starttime = Engine::getTime();
	}
	unsigned int Timer::getTime()
	{
		unsigned int time = this->time;
		if (running)
		{
			time += (Engine::getTime() - starttime) / 1000;
		}
		return time;
	}

	void Timer::start()
	{
		stop();
		running = true;
		timers.push_back(this);
		starttime = Engine::getTime();
	}
	void Timer::stop()
	{
		if (running)
		{
			time += (Engine::getTime() - starttime) / 1000;
			for (unsigned int i = 0; i < timers.size(); i++)
			{
				if (timers[i] == this)
				{
					timers.erase(timers.begin() + i);
					break;
				}
			}
		}
		running = false;
	}
	void Timer::reset()
	{
		setTime(0);
	}
	bool Timer::isRunning()
	{
		return running;
	}

	void Timer::setCallback(ScriptPointer script, std::string function)
	{
		this->script = script;
		this->function = function;
	}

	void Timer::callCallbacks()
	{
		for (unsigned int i = 0; i < timers.size(); i++)
		{
			unsigned int time = timers[i]->getTime();
			if (time >= timers[i]->interval)
			{
				// Reset time
				time %= timers[i]->interval;
				timers[i]->setTime(time);
				// Call callback
				if (timers[i]->script && timers[i]->function != "")
				{
					if (timers[i]->script->isFunction(timers[i]->function))
						timers[i]->script->callFunction(timers[i]->function);
					else
						std::cout << "Timer: Function not present!" << std::endl;
				}
				// Stop timer
				if (!timers[i]->periodic)
				{
					timers[i]->reset();
					timers[i]->running = false;
					timers.erase(timers.begin() + i);
					i--;
				}
			}
		}
	}

	std::vector<Timer*> Timer::timers;
}
