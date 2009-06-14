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

#ifndef _ENTITYTEMPLATE_HPP_
#define _ENTITYTEMPLATE_HPP_

#include "ReferenceCounted.hpp"
#include "entity/Property.hpp"

#include <string>
#include <vector>
#include <map>

namespace backlot
{
	class EntityTemplate : public ReferenceCounted
	{
		public:
			EntityTemplate();
			~EntityTemplate();

			static SharedPointer<EntityTemplate> get(std::string name);

			bool load(std::string name);

			const std::vector<Property> &getProperties();
			const std::string &getScript();
			const Vector2F &getSize();
			const Vector2F &getOrigin();
		private:
			std::string name;

			std::vector<Property> properties;
			std::string script;
			Vector2F size;
			Vector2F origin;
			bool blocking;

			static std::map<std::string, EntityTemplate*> templates;
	};

	typedef SharedPointer<EntityTemplate> EntityTemplatePointer;
}

#endif
