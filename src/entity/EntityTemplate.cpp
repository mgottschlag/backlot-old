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
	EntityTemplate::EntityTemplate() : ReferenceCounted()
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
		TiXmlNode *node = xml.FirstChild("entity");
		if (!node)
		{
			std::cerr << "Parser error: <entity> not found." << std::endl;
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
		TiXmlNode *propertynode = properties->FirstChild();
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
			propertynode = properties->IterateChildren(propertynode);
		}
		// Load script
		TiXmlNode *scriptnode = root->FirstChild("script");
		while (scriptnode)
		{
			TiXmlElement *scriptdata = scriptnode->ToElement();
			if (scriptdata)
			{
				TiXmlNode *textnode = scriptdata->FirstChild();
				while (textnode)
				{
					TiXmlText *text = textnode->ToText();
					if (text && text->Value())
					{
						script += text->Value();
					}
					textnode = scriptdata->IterateChildren(textnode);
				}
			}
			scriptnode = node->IterateChildren("script", scriptnode);
		}
		#ifdef CLIENT
		// Load image information
		TiXmlNode *imagenode = root->FirstChild("image");
		while (imagenode)
		{
			TiXmlElement *imagedata = imagenode->ToElement();
			if (imagedata)
			{
				EntityImageInfo image;
				// Image name
				if (imagedata->Attribute("name"))
					image.name = imagedata->Attribute("name");
				// Texture file
				if (imagedata->Attribute("src"))
				{
					image.texture = new Texture();
					if (!image.texture->load(imagedata->Attribute("src")))
						image.texture = 0;
				}
				// Position relative to the entity
				if (imagedata->Attribute("position"))
					image.position = imagedata->Attribute("position");
				// Size
				if (imagedata->Attribute("size"))
					image.size = imagedata->Attribute("size");
				// Depth relative to the entity
				if (imagedata->Attribute("depth"))
				{
					double depth;
					imagedata->Attribute("depth", &depth);
					image.depth = depth;
				}
				else
					image.depth = 0;
				// If set to true, the image rotates with the entity
				if (imagedata->Attribute("rotate"))
					image.rotate = strcmp(imagedata->Attribute("rotate"), "no");
				else
					image.rotate = false;
				images.push_back(image);
			}
			imagenode = node->IterateChildren("image", imagenode);
		}
		#endif
		// Add to list
		this->name = name;
		templates.insert(std::pair<std::string, EntityTemplate*>(name, this));
		return true;
	}
	std::string EntityTemplate::getName()
	{
		return name;
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
	#ifdef CLIENT
	const std::vector<EntityImageInfo> &EntityTemplate::getImages()
	{
		return images;
	}
	#endif

	std::map<std::string, EntityTemplate*> EntityTemplate::templates;
}
