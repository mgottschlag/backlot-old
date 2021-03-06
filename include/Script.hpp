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
#include <luabind/luabind.hpp>

namespace backlot
{
	/**
	 * Lua script. Parts of the engine are exported into the lua VM, note
	 * however that you have to pass reference counted objects as shared
	 * pointers or the script will crash.
	 */
	class Script: public ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 */
			Script();
			/**
			 * Destructor.
			 */
			~Script();

			/**
			 * Executes a string containing lua code within the Lua VM.
			 */
			bool runString(std::string data);

			/**
			 * Returns true if a function with the given name exists in the
			 * global scope.
			 */
			bool isFunction(std::string name);
			/**
			 * Calls the function with the given name without any parameters.
			 */
			void callFunction(std::string name);

			// Functions without return value
			template <typename A> void callFunction(std::string name, A arg1)
			{
				luabind::call_function<void>(state, name.c_str(), arg1);
			};
			template <typename A1, typename A2> void callFunction(std::string name, A1 arg1, A2 arg2)
			{
				luabind::call_function<void>(state, name.c_str(), arg1, arg2);
			};
			template <typename A1, typename A2, typename A3> void callFunction(std::string name, A1 arg1, A2 arg2, A3 arg3)
			{
				luabind::call_function<void>(state, name.c_str(), arg1, arg2, arg3);
			};
			template <typename A1, typename A2, typename A3, typename A4> void callFunction(std::string name, A1 arg1, A2 arg2, A3 arg3, A4 arg4)
			{
				luabind::call_function<void>(state, name.c_str(), arg1, arg2, arg3, arg4);
			};

			// Functions with return value
			template <typename R> R callFunction(std::string name)
			{
				return luabind::call_function<R>(state, name.c_str());
			};
			template <typename R, typename A> R callFunction(std::string name, A arg1)
			{
				return luabind::call_function<R>(state, name.c_str(), arg1);
			};
			template <typename R, typename A1, typename A2> R callFunction(std::string name, A1 arg1, A2 arg2)
			{
				return luabind::call_function<R>(state, name.c_str(), arg1, arg2);
			};
			template <typename R, typename A1, typename A2, typename A3> R callFunction(std::string name, A1 arg1, A2 arg2, A3 arg3)
			{
				return luabind::call_function<R>(state, name.c_str(), arg1, arg2, arg3);
			};

			/**
			 * Sets a global variable to the given value. This can be any core
			 * type or any registered class. If the name contains dots, tables
			 * are created accordingly (e.g. "table.table2.variable").
			 */
			template <typename T> void setVariable(std::string name, T value)
			{
				luabind::object table = luabind::globals(state);
				while (name.find('.') != std::string::npos)
				{
					std::string tablename = name.substr(0, name.find('.'));
					name = name.substr(name.find('.') + 1);
					table = table[tablename.c_str()];
					if (luabind::type(table) != LUA_TTABLE)
						table = luabind::newtable(state);
				}
				table[name.c_str()] = value;
			};

			void addCoreFunctions();
			#ifdef CLIENT
			void addClientFunctions();
			void addDialogFunctions();
			#endif
			#ifdef SERVER
			void addServerFunctions();
			#endif
		private:
			lua_State *state;
	};

	typedef SharedPointer<Script> ScriptPointer;
}

#endif
