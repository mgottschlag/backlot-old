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

#ifndef _RECTANGLE_HPP_
#define _RECTANGLE_HPP_

#include <string>
#include <cstdlib>

namespace backlot
{
	template<typename T> class Rectangle
	{
		public:
			Rectangle()
			{
				x = 0;
				y = 0;
				width = 0;
				height = 0;
			}
			Rectangle(T x, T y, T width, T height)
			{
				this->x = x;
				this->y = y;
				this->width = width;
				this->height = height;
			}
			Rectangle(const Rectangle& r)
			{
				x = r.x;
				y = r.y;
				width = r.width;
				height = r.height;
			}
			Rectangle(const char *s)
			{
				set(s);
			}
			Rectangle(const std::string &s)
			{
				set(s);
			}
			~Rectangle()
			{
			}

			template<typename T2> Rectangle<T> &operator=(const Rectangle<T2> &r)
			{
				x = r.x;
				y = r.y;
				width = r.width;
				height = r.height;
				return *this;
			}
			template<typename T2> Rectangle<T> &operator=(std::string s)
			{
				set(s);
				return *this;
			}
			template<typename T2> Rectangle<T> &operator=(const char *s)
			{
				set(s);
				return *this;
			}

			void set(const std::string &s)
			{
				size_t separator1 = s.find("/");
				size_t separator2 = s.find("/", separator1 + 1);
				size_t separator3 = s.find("/", separator2 + 1);
				if ((separator1 == std::string::npos)
					|| (separator2 == std::string::npos)
					|| (separator3 == std::string::npos))
				{
					x = 0;
					y = 0;
					width = 0;
					height = 0;
				}
				else
				{
					x = atof(s.c_str());
					const char *ys = s.c_str() + separator1 + 1;
					y = atof(ys);
					const char *widths = s.c_str() + separator2 + 1;
					width = atof(widths);
					const char *heights = s.c_str() + separator3 + 1;
					height = atof(heights);
				}
			}

			T x;
			T y;
			T width;
			T height;
	};

	typedef Rectangle<int> RectangleI;
	typedef Rectangle<float> RectangleF;
}

#endif
