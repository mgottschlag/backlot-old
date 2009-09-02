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

#include "overlay/OverlayGroup.hpp"
#include "overlay/OverlayImage.hpp"

#include <iostream>

namespace backlot
{
	OverlayGroup::OverlayGroup()
	{
	}
	OverlayGroup::~OverlayGroup()
	{
	}
	
	bool OverlayGroup::load(TiXmlNode *xmlnode)
	{
		TiXmlNode *itemnode;
		TiXmlElement *itemelement;
		
		// Parse images
		itemnode = xmlnode->FirstChild("image");
		while(itemnode)
		{
			itemnode = xmlnode->IterateChildren("image", itemnode);
			if(itemnode)
			{
				itemelement = itemnode->ToElement();
				// Check for attributes
				if(!itemelement->Attribute("name"))
				{
					std::cerr << "No \"name\" found in <image>" << std::endl;
					return false;
				}
				if(!itemelement->Attribute("offset") || !itemelement->Attribute("size"))
				{
					std::cerr << "No \"offset\" and/or \"size\" found in <image>" << std::endl;
					return false;
				}
				if(!itemelement->Attribute("image-path") || !itemelement->Attribute("texture-size"))
				{
					std::cerr << "No \"image-path\" and/or \"texture-size\" found in <image>" << std::endl;
					return false;
				}
				// Instantiate new OverlayItem object
				std::string itemname = itemelement->Attribute("name");
				OverlayImage *image = new OverlayImage();
				image->setName(itemname);
				image->setSize( Vector2I(itemelement->Attribute("size")) );
				image->setOffset( Vector2I(itemelement->Attribute("offset")) );
				image->loadImage(itemelement->Attribute("image-path"),
					Vector2I(itemelement->Attribute("texture-size")));
				items[itemname] = image;
				
				// Jump to the next <image> entry
				itemnode = xmlnode->IterateChildren("image", itemnode);
			}

		}
		return true;
	}
}
