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

#ifndef _OVERLAYITEM_HPP_
#define _OVERLAYITEM_HPP_

#include "ReferenceCounted.hpp"
#include "Vector2.hpp"
#include "tinyxml.h"

namespace backlot
{
	class OverlayItem : public ReferenceCounted
	{
		public:
			OverlayItem();
			~OverlayItem();
			
			void setName(std::string name);
			std::string getName();
			void setPosition(Vector2I position);
			Vector2I getPosition();
			void setOffset(Vector2I offset);
			Vector2I getOffset();
			void setSize(Vector2I size);
			Vector2I getSize();
			
			/**
			 * Load the item from a TiXmlNode.
			 */
			bool load(TiXmlNode *xmlnode);
			
			virtual void render();
		protected:
			std::string name;
			Vector2I position;
			Vector2I offset;
			Vector2I size;
	};

	typedef SharedPointer<OverlayItem> OverlayItemPointer;
}

#endif
