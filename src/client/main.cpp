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

#include "Engine.hpp"

#include <iostream>
#include <luabind/luabind.hpp>
#include <guichan/exception.hpp>

int main(int argc, char **argv)
{
	// Parse arguments
	if (argc == 1)
	{
		std::cerr << "Error: No game directory given." << std::endl;
		return -1;
	}
	std::vector<std::string> args;
	for (int i = 2; i < argc; i++)
		args.push_back(argv[i]);
	// Run game
	try
	{
		if (!backlot::Engine::get().run(argv[1], args))
		{
			std::cerr << "Completed with errors." << std::endl;
			return -1;
		}
	}
	catch (luabind::error &e)
	{
		lua_State *state = e.state();
		std::cerr << "Script exception: " << e.what() << std::endl;
		if (lua_isstring(state, -1))
		{
			std::cerr << lua_tostring(state, -1) << std::endl;
		}
		else
		{
			std::cerr << "No valid error message!" << std::endl;
		}
		return -1;
	}
	catch (gcn::Exception &e)
	{
		std::cerr << "Guichan exception: " << e.getMessage() << std::endl;
		return -1;
	}
	return 0;
}
