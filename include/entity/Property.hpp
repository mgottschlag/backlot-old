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

#ifndef _PROPERTY_HPP_
#define _PROPERTY_HPP_

#include "Vector2.hpp"

#include <string>

namespace backlot
{
	enum PropertyType
	{
		EPT_Integer,
		EPT_Float,
		EPT_Vector2F
	};

	enum PropertyFlags
	{
		EPF_None = 0x0,
		EPF_Predict = 0x1,
		EPF_OwnerUpdates = 0x2
	};

	class Property
	{
		public:
			Property();
			Property(std::string name, PropertyType type,
				PropertyFlags flags = EPF_None);
			Property(const Property &property);
			~Property();

			void setName(std::string name);
			std::string getName();
			void setType(PropertyType type);
			PropertyType getType();
			void setFlags(PropertyFlags flags);
			PropertyFlags getFlags();

			void setSize(unsigned int size);
			unsigned int getSize();

			void setInt(int data);
			int getInt();
			void setUnsignedInt(unsigned int data);
			unsigned int getUnsignedInt();
			void setFloat(float data);
			float getFloat();
			void setVector(const Vector2F &data);
			Vector2F getVector();

			Property &operator=(const Property &property);
			bool operator==(const Property &property);
		private:
			std::string name;
			PropertyType type;
			PropertyFlags flags;
			unsigned int size;
			char data[8];
	};
}

#endif
