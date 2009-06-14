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

#include "entity/EntityTemplate.hpp"
#include "Engine.hpp"

#include "support/tinyxml.h"

#include <iostream>

namespace backlot
{
	EntityTemplate::EntityTemplate()
	{
	}
	EntityTemplate::~EntityTemplate()
	{
		if (name != "")
		{
			// Remove from loaded templates
			std::map<std::string, EntityTemplate*>::iterator it = templates.find(name);
			if (it != templates.end())
			{
				templates.erase(it);
			}
		}
	}

	SharedPointer<EntityTemplate> EntityTemplate::get(std::string name)
	{
		// Get already loaded template
		std::map<std::string, EntityTemplate*>::iterator it = templates.find(name);
		if (it != templates.end())
		{
			return it->second;
		}
		// Load map
		EntityTemplatePointer tpl = new EntityTemplate();
		if (!tpl->load(name))
		{
			return 0;
		}
		return tpl;
	}

	bool EntityTemplate::load(std::string name)
	{
		// Open XML file
		std::string filename = Engine::get().getGameDirectory() + "/entities/" + name + ".xml";
		TiXmlDocument xml(filename.c_str());
		if (!xml.LoadFile() || xml.Error())
		{
			std::cerr << "Could not load XML file " << name << ".xml: " << xml.ErrorDesc() << std::endl;
			return false;
		}
		TiXmlNode *node = xml.FirstChild("weapon");
		if (!node)
		{
			std::cerr << "Parser error: <weapon> not found." << std::endl;
			return false;
		}
		TiXmlElement *root = node->ToElement();
		// Rectangle
		if (root->Attribute("size"))
		{
			size = root->Attribute("size");
		}
		if (root->Attribute("origin"))
		{
			size = root->Attribute("origin");
		}
		blocking = false;
		if (root->Attribute("blocking") && !strcmp(root->Attribute("blocking"), "yes"))
		{
			blocking = true;
		}
		// Parse properties
		TiXmlElement *properties = 0;
		TiXmlNode *propertiesnode = root->FirstChild("properties");
		while (propertiesnode)
		{
			properties = propertiesnode->ToElement();
			if (properties)
				break;
			propertiesnode = root->IterateChildren("properties", propertiesnode);
		}
		if (!properties)
		{
			std::cerr << "Parser error: <properties> not found." << std::endl;
			return false;
		}
		TiXmlNode *propertynode = root->FirstChild();
		while (propertynode)
		{
			TiXmlElement *property = propertynode->ToElement();
			if (property)
			{
				std::string propname = property->Value();
				std::cout << "Property " << propname << "." << std::endl;
				// Get property info
				PropertyType type = EPT_Integer;
				const char *typestr = property->Attribute("type");
				if (!typestr)
				{
					std::cerr << "Property " << propname << " does not have any type!" << std::endl;
					return false;
				}
				if (!strcmp(typestr, "int") || !strcmp(typestr, "uint"))
				{
					type = EPT_Integer;
				}
				else if (!strcmp(typestr, "float"))
				{
					type = EPT_Float;
				}
				else if (!strcmp(typestr, "vector2i"))
				{
					type = EPT_Vector2I;
				}
				else if (!strcmp(typestr, "vector2f"))
				{
					type = EPT_Vector2F;
				}
				int size = 32;
				if (property->Attribute("size"))
					property->Attribute("size", &size);
				// Create property
				this->properties.push_back(Property(propname, type));
				Property &newprop = this->properties[this->properties.size() - 1];
				newprop.setSize(size);
				if (property->Attribute("default"))
					newprop.set(property->Attribute("default"));
			}
			propertiesnode = root->IterateChildren(propertiesnode);
		}
		// Add to list
		this->name = name;
		templates.insert(std::pair<std::string, EntityTemplate*>(name, this));
		return true;
	}

	const std::vector<Property> &EntityTemplate::getProperties()
	{
		return properties;
	}
	const std::string &EntityTemplate::getScript()
	{
		return script;
	}
	const Vector2F &EntityTemplate::getSize()
	{
		return size;
	}
	const Vector2F &EntityTemplate::getOrigin()
	{
		return origin;
	}

	std::map<std::string, EntityTemplate*> EntityTemplate::templates;
}
