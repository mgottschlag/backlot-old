/*
Copyright (C) 2009 Stephan Jauernick

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

#include <iostream>
#include <QCoreApplication>
#include "Server.cpp"
	

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);

	std::cout << "Backlot MasterServer PID:" << app.applicationPid()  << std::endl;
	backlot::Server server = new backlot::Server();
//	test.print_infos();

//	if(test.ping())
//	{
//		std::cout << "Server online" << std::endl;
//	}
//	else
//	{
//		std::cout << "Server offline" << std::endl;
//	}
//	
//	test.print_infos();
//
//
//	if(test.ping())
//	{
//		std::cout << "Server online" << std::endl;
//	}
//	else
//	{
//		std::cout << "Server offline" << std::endl;
//	}
//
//	test.print_infos();
//
//	if(test.ping())
//	{
//		std::cout << "Server online" << std::endl;
//	}
//	else
//	{
//		std::cout << "Server offline" << std::endl;
//	}
////	test.print_infos();
			
	return app.exec();
}
