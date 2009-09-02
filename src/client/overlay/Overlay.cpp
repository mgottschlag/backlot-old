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

#include "overlay/Overlay.hpp"
#include "Engine.hpp"

#include <iostream>
#include <guichan/widgets/label.hpp>

namespace backlot
{

	Overlay::Overlay()
	{
	}
	Overlay::~Overlay()
	{
	}
	
	SharedPointer<Overlay> Overlay::get(std::string name)
	{
		// Get already loaded overlay
		std::map<std::string, Overlay*>::iterator it = overlays.find(name);
		if (it != overlays.end())
		{
			return it->second;
		}
		// Load overlay
		OverlayPointer overlay = new Overlay();
		if (!overlay->load(name))
		{
			return 0;
		}
		return overlay;
	}
	
	bool Overlay::load(std::string name)
	{
		std::string fullpath = Engine::get().getGameDirectory() + "/overlays/" + name;
		TiXmlDocument doc(fullpath.c_str());
		if(!doc.LoadFile())
		{
			std::cerr << "Could not find overlay file " << fullpath << std::endl;
			return false;
		}
		TiXmlNode *xmlroot;
		TiXmlNode *xmlnode;
		TiXmlElement *xmlelement;
		if((xmlroot = doc.FirstChild("overlay")))
		{
			std::cerr << "Could not find <overlay> in " << fullpath << std::endl;
			return false;
		}
		xmlelement = xmlroot->ToElement();
		if(!xmlelement->Attribute("name"))
		{
			std::cerr << "Could not find \"name\" for <overlay> in overlay file " << fullpath << std::endl;
			return false;
		}
		name = xmlelement->Attribute("name");
		
		// Parsing groups
		xmlnode = xmlroot->FirstChild("group");
		while(xmlnode)
		{
			if((xmlelement = xmlnode->ToElement()))
			{
				if(!xmlelement->Attribute("name"))
				{
					std::cerr << "Could not find \"name\" for <group> in overlay file " << fullpath << std::endl;
					return false;
				}
				// Instantiate new group
				groups[xmlelement->Attribute("name")] = new OverlayGroup;
				// Load group from node
				if(!groups[xmlelement->Attribute("name")]->load(xmlnode))
				{
					std::cout << "Error on loading overlay group from " << fullpath << std::endl;
					return false;
				}
			}

			xmlnode = xmlroot->IterateChildren("group", xmlnode);
		}
		
		return true;
	}
	
	std::map<std::string, Overlay*> Overlay::overlays;
}
