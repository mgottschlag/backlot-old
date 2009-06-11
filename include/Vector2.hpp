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

#ifndef _VECTOR2_HPP_
#define _VECTOR2_HPP_

#include <string>
#include <cstdlib>

namespace backlot
{
	/**
	 * Utility class for 2D vectors.
	 */
	template<typename T> class Vector2
	{
		public:
			/**
			 * Constructor.
			 */
			Vector2()
			{
				x = 0;
				y = 0;
			}
			/**
			 * Constructor.
			 */
			Vector2(T x, T y)
			{
				this->x = x;
				this->y = y;
			}
			/**
			 * Constructor.
			 */
			Vector2(const char *s)
			{
				set(s);
			}
			/**
			 * Constructor.
			 */
			Vector2(const std::string &s)
			{
				set(s);
			}
			/**
			 * Constructor.
			 */
			template<typename T2> Vector2(const Vector2<T2> &v)
			{
				x = v.x;
				y = v.y;
			}
			/**
			 * Destructor.
			 */
			~Vector2()
			{
			}

			template<typename T2> Vector2<T> operator*(T2 s) const
			{
				return Vector2<T>(x * s, y * s);
			}
			template<typename T2> Vector2<T> &operator*=(T2 s)
			{
				x = x * s;
				y = y * s;
				return *this;
			}
			template<typename T2> Vector2<T> operator+(const Vector2<T2> &v) const
			{
				return Vector2<T>(x + v.x, y + v.y);
			}
			template<typename T2> Vector2<T> &operator=(const Vector2<T2> &v)
			{
				x = v.x;
				y = v.y;
				return *this;
			}
			Vector2<T> &operator=(std::string s)
			{
				set(s);
				return *this;
			}
			Vector2<T> &operator=(const char *s)
			{
				set(s);
				return *this;
			}
			template<typename T2> Vector2<T> &operator+=(const Vector2<T2> &v)
			{
				x += v.x;
				y += v.y;
				return *this;
			}
			template<typename T2> bool operator==(const Vector2<T2> &v) const
			{
				return x == v.x && y == v.y;
			}
			template<typename T2> bool operator!=(const Vector2<T2> &v) const
			{
				return x != v.x || y != v.y;
			}

			void set(const std::string &s)
			{
				size_t separator = s.find("/");
				if (separator == std::string::npos)
				{
					x = 0;
					y = 0;
				}
				else
				{
					x = atof(s.c_str());
					const char *ys = s.c_str() + separator + 1;
					y = atof(ys);
				}
			}

			/**
			 * X coordinate of the vector.
			 */
			T x;
			/**
			 * Y coordinate of the vector.
			 */
			T y;
	};

	typedef Vector2<int> Vector2I;
	typedef Vector2<float> Vector2F;
}

#endif
