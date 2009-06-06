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

#include "Server.hpp"
#include "Engine.hpp"

#include <iostream>
#include <cstring>
#if defined(_MSC_VER) || defined(_WINDOWS_) || defined(_WIN32)
#include <tchar.h>
#else
#include <signal.h>
#endif

namespace backlot
{
	Server &Server::get()
	{
		static Server server;
		return server;
	}
	Server::~Server()
	{
	}

	bool Server::init(int port, std::string mapname, int maxclients)
	{
		// Start server
		#if defined(_MSC_VER) || defined(_WINDOWS_) || defined(_WIN32)
		HANDLE inread = 0;
		HANDLE inwrite = 0;
		HANDLE outread = 0;
		HANDLE outwrite = 0;

		// Inherit pipes
		SECURITY_ATTRIBUTES secattr;
		secattr.nLength = sizeof(SECURITY_ATTRIBUTES);
		secattr.bInheritHandle = TRUE;
		secattr.lpSecurityDescriptor = NULL;
		// Create pipes
		if (!CreatePipe(&outread, &outwrite, &secattr, 0)
			|| !CreatePipe(&inread, &inwrite, &secattr, 0))
		{
			std::cerr << "Could not create pipes." << std::endl;
			return false;
		}
		// Don't inherit unused ends
		SetHandleInformation(inread, HANDLE_FLAG_INHERIT, 0);
		SetHandleInformation(outwrite, HANDLE_FLAG_INHERIT, 0);
		// Create process
		TCHAR cmdline[256];
		_sntprintf(cmdline, 256, TEXT("./server.exe %s %s"),
			Engine::get().getGameDirectory().c_str(), mapname.c_str());
		PROCESS_INFORMATION procinfo;
		STARTUPINFO startinfo;
		ZeroMemory(&procinfo, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&startinfo, sizeof(STARTUPINFO));
		startinfo.cb = sizeof(STARTUPINFO);
		startinfo.hStdError = inwrite;
		startinfo.hStdOutput = inwrite;
		startinfo.hStdInput = outread;
		startinfo.dwFlags |= STARTF_USESTDHANDLES;
		if (CreateProcess(NULL, cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &startinfo, &procinfo))
		{
			process = procinfo.hProcess;
			CloseHandle(procinfo.hThread);
		}
		else
		{
			std::cerr << "Could not start server." << std::endl;
			return false;
		}
		// Wait for server to be ready
		while (1)
		{
			char line[512];
			memset(line, 0, 512);
			int bytesread = 0;
			DWORD byteread = 0;
			// Read line from pipe
			while (1)
			{
				if (!ReadFile(inread, line + bytesread, 1, &byteread, NULL) || !byteread)
				{
					std::cerr << "Server closed unexpectedly." << std::endl;
					return false;
				}
				if (line[bytesread] == '\n')
				{
					line[bytesread - 1] = 0;
					break;
				}
				if (bytesread == 510)
				{
					line[511] = 0;
					break;
				}
				bytesread++;
			}
			// Wait for "ready"
			std::cout << "Server: \"" << line << "\"" << std::endl;
			if (!strcmp(line, "ready"))
			{
				break;
			}
		}
		#else
		// Create pipes for connection to the server
		int serverin[2];
		int serverout[2];
		pipe(serverin);
		pipe(serverout);
		pid = fork();
		if (pid == -1)
		{
			std::cerr << "Could not fork program." << std::endl;
			return false;
		}
		if (pid)
		{
			// Client
			close(serverin[0]);
			close(serverout[1]);
			out = serverin[1];
			in = serverout[0];
		}
		else
		{
			// Server
			close(serverin[1]);
			close(serverout[0]);
			dup2(serverin[0], 0);
			dup2(serverout[1], 1);
			dup2(serverout[1], 2);
			// Start server
			if (execlp("./server", "./server", Engine::get().getGameDirectory().c_str(),
				mapname.c_str(), NULL) == -1)
			{
				std::cerr << "error: Could not start server.\n" << std::endl;
				exit(-1);
			}
		}
		// Wait for server to be ready
		infile = fdopen(in, "r");
		bool success = true;
		while (1)
		{
			char msg[512];
			if (!fgets(msg, 512, infile))
			{
				// Server closed unexpectedly
				success = false;
				break;
			}
			msg[strlen(msg) - 1] = 0;
			std::cout << "Server: \"" << msg << "\"" << std::endl;
			if (!strcmp(msg, "ready"))
			{
				break;
			}
		}
		if (!success)
		{
			std::cerr << "Error while starting server." << std::endl;
			fclose(infile);
			close(in);
			close(out);
			return false;
		}
		std::cout << "Server started." << std::endl;
		#endif
		return true;
	}
	bool Server::destroy()
	{
		#if defined(_MSC_VER) || defined(_WINDOWS_) || defined(_WIN32)
		TerminateProcess(process, 0);
		CloseHandle(process);
		#else
		// TODO: We're a little rough here
		kill(pid, SIGKILL);
		fclose(infile);
		close(in);
		close(out);
		#endif
		return false;
	}

	bool Server::update()
	{
		return true;
	}

	Server::Server()
	{
	}
}
