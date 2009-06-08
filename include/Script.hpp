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

#ifndef _SCRIPT_HPP_
#define _SCRIPT_HPP_

#include "ReferenceCounted.hpp"

#include <string>
extern "C"
{
	#include "lua.h"
}

namespace backlot
{
	class Script: public ReferenceCounted
	{
		public:
			Script();
			~Script();

			bool runString(std::string data);

			bool isFunction(std::string name);
			void callFunction(std::string name);

			// Functions without return value
			template <typename A> void callFunction(std::string name, A arg1)
			{
				lua_getglobal(state, name.c_str());
				push(arg1);
				lua_call(state, 1, 0);
			};
			template <typename A1, typename A2> void callFunction(std::string name, A1 arg1, A2 arg2)
			{
				lua_getglobal(state, name.c_str());
				push(arg1);
				push(arg2);
				lua_call(state, 2, 0);
			};
			template <typename A1, typename A2, typename A3> void callFunction(std::string name, A1 arg1, A2 arg2, A3 arg3)
			{
				lua_getglobal(state, name.c_str());
				push(arg1);
				push(arg2);
				push(arg3);
				lua_call(state, 3, 0);
			};
			template <typename A1, typename A2, typename A3, typename A4> void callFunction(std::string name, A1 arg1, A2 arg2, A3 arg3, A4 arg4)
			{
				lua_getglobal(state, name.c_str());
				push(arg1);
				push(arg2);
				push(arg3);
				push(arg4);
				lua_call(state, 4, 0);
			};

			// Functions with return value
			template <typename R> R callFunction(std::string name)
			{
				lua_getglobal(state, name.c_str());
				lua_call(state, 0, 1);
				R value;
				pop(value);
				return value;
			};
			template <typename R, typename A> R callFunction(std::string name, A arg1)
			{
				lua_getglobal(state, name.c_str());
				push(arg1);
				lua_call(state, 1, 1);
				R value;
				pop(value);
				return value;
			};
			template <typename R, typename A1, typename A2> R callFunction(std::string name, A1 arg1, A2 arg2)
			{
				lua_getglobal(state, name.c_str());
				push(arg1);
				push(arg2);
				lua_call(state, 2, 1);
				R value;
				pop(value);
				return value;
			};
			template <typename R, typename A1, typename A2, typename A3> R callFunction(std::string name, A1 arg1, A2 arg2, A3 arg3)
			{
				lua_getglobal(state, name.c_str());
				push(arg1);
				push(arg2);
				push(arg3);
				lua_call(state, 3, 1);
				R value;
				pop(value);
				return value;
			};

			void addCoreFunctions();
			#ifdef CLIENT
			void addMenuFunctions();
			#endif
		private:
			void push(int value);
			void push(float value);
			void push(double value);
			void push(std::string value);

			void pop(int &value);
			void pop(float &value);
			void pop(double &value);
			void pop(std::string &value);

			lua_State *state;
	};

	typedef SharedPointer<Script> ScriptPointer;
}

#endif
