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

#ifndef _OVERLAY_HPP_
#define _OVERLAY_HPP_

#include "overlay/OverlayGroup.hpp"
#include "Script.hpp"
#include "ReferenceCounted.hpp"
#include "tinyxml.h"

#include <string>
#include <map>

namespace backlot
{
	class Overlay : public ReferenceCounted
	{
		public:
			Overlay();
			~Overlay();
			
			static SharedPointer<Overlay> get(std::string name);
			/**
			 * Parses through a .xml file and loads the overlay.
			 * @param filepath The .xml file from which the overlay is loaded.
			 * @return True if the overlay was loaded correctly.
			 */
			bool load(std::string filepath);
			
		private:
			std::string name;
			ScriptPointer script;
			std::map<std::string, OverlayGroupPointer> groups;
			
			static std::map<std::string, Overlay*> overlays;
	};

	typedef SharedPointer<Overlay> OverlayPointer;
}

#endif
