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

#include "Line.hpp"

#include <string>
#include <cstdlib>

namespace backlot
{
	/**
	 * Utility class representing a 2D rectangle. The rectangle is defined
	 * through position and size.
	 */
	template<typename T> class Rectangle
	{
		public:
			/**
			 * Constructor.
			 */
			Rectangle()
			{
				x = 0;
				y = 0;
				width = 0;
				height = 0;
			}
			/**
			 * Constructor.
			 */
			Rectangle(T x, T y, T width, T height)
			{
				this->x = x;
				this->y = y;
				this->width = width;
				this->height = height;
			}
			/**
			 * Constructor.
			 */
			Rectangle(const Vector2F &position, const Vector2F &size)
			{
				this->x = position.x;
				this->y = position.y;
				this->width = size.x;
				this->height = size.y;
			}
			/**
			 * Constructor.
			 */
			Rectangle(const Rectangle& r)
			{
				x = r.x;
				y = r.y;
				width = r.width;
				height = r.height;
			}
			/**
			 * Constructor.
			 */
			Rectangle(const char *s)
			{
				set(s);
			}
			/**
			 * Constructor.
			 */
			Rectangle(const std::string &s)
			{
				set(s);
			}
			/**
			 * Destructor.
			 */
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
					x = (T)atof(s.c_str());
					const char *ys = s.c_str() + separator1 + 1;
					y = (T)atof(ys);
					const char *widths = s.c_str() + separator2 + 1;
					width = (T)atof(widths);
					const char *heights = s.c_str() + separator3 + 1;
					height = (T)atof(heights);
				}
			}

			/**
			 * Computes the intersections with a line. If start or end are
			 * within the rectangle, they are counted as intersections.
			 * @param line Line to be used for the intersection check.
			 * @param intersection1 First intersection with the rectangle
			 * (nearer to start than intersection2).
			 * @param intersection2 Second intersection with the rectangle.
			 * @return True, if the line intersects with the rectangle.
			 */
			bool getIntersections(const Line &line,
				Vector2F &intersection1, Vector2F &intersection2)
			{
				// TODO: In the corners we might get more than 2 intersections
				int intersectioncount = 0;
				// Get rectangle edges
				Line edge1(Vector2F(x, y), Vector2F(x, y + height));
				Line edge2(Vector2F(x, y), Vector2F(x + width, y));
				Line edge3(Vector2F(x, y + height), Vector2F(x + width, y + height));
				Line edge4(Vector2F(x + width, y), Vector2F(x + width, y + height));
				// Get intersections with the line
				Vector2F intersection;
				if (edge1.getIntersection(line, intersection))
				{
					intersectioncount++;
					intersection1 = intersection;
				}
				if (edge2.getIntersection(line, intersection))
				{
					if (intersectioncount == 0)
					{
						intersection1 = intersection;
						intersectioncount++;
					}
					else if (intersectioncount == 1 && intersection != intersection1)
					{
						intersection2 = intersection;
						intersectioncount++;
					}
				}
				if (edge3.getIntersection(line, intersection))
				{
					if (intersectioncount == 0)
					{
						intersection1 = intersection;
						intersectioncount++;
					}
					else if (intersectioncount == 1 && intersection != intersection1)
					{
						intersection2 = intersection;
						intersectioncount++;
					}
				}
				if (edge4.getIntersection(line, intersection))
				{
					if (intersectioncount == 0)
					{
						intersection1 = intersection;
						intersectioncount++;
					}
					else if (intersectioncount == 1 && intersection != intersection1)
					{
						intersection2 = intersection;
						intersectioncount++;
					}
				}
				// Return result
				if (intersectioncount == 2)
				{
					// Order intersections
					float dist1 = (intersection1.x - line.start.x)
						* (intersection1.x - line.start.x)
						+ (intersection1.y - line.start.y)
						* (intersection1.y - line.start.y);
					float dist2 = (intersection2.x - line.start.x)
						* (intersection2.x - line.start.x)
						+ (intersection2.y - line.start.y)
						* (intersection2.y - line.start.y);
					if (dist2 < dist1)
					{
						Vector2F tmp = intersection1;
						intersection1 = intersection2;
						intersection2 = tmp;
					}
					return true;
				}
				else if (intersectioncount == 1)
				{
					// Check start and end
					// TODO
					intersection2 = intersection1;
					return true;
				}
				return false;
			}

			/**
			 * Enlarges the rectangle so that the point fits into it.
			 */
			void insertPoint(const Vector2<T> &point)
			{
				if (x > point.x)
				{
					width += x - point.x;
					x = point.x;
				}
				if (y > point.y)
				{
					height += y - point.y;
					y = point.y;
				}
				if (x + width < point.x)
				{
					width = point.x - x;
				}
				if (y + height < point.y)
				{
					height = point.y - y;
				}
			}
			/**
			 * Enlarges the rectangle so that the rectangle fits into it.
			 */
			void insertRectangle(const Rectangle<T> &rect)
			{
				// Insert two points which cover the whole rectangle
				insertPoint(Vector2<T>(rect.x, rect.y));
				insertPoint(Vector2<T>(rect.x + width, rect.y + height));
			}
			/**
			 * Checks whether the point is within the rectangle.
			 */
			bool contains(const Vector2<T> &point) const
			{
				return point.x >= x && point.y >= y && point.x < x + width
					&& point.y < y + height;
			}
			/**
			 * Checks whether the rectangles overlap.
			 */
			bool overlapsWith(const Rectangle<T> &rect) const
			{
				// One of the corners of rect is in this rectangle?
				if (contains(Vector2<T>(rect.x, rect.y)))
					return true;
				if (contains(Vector2<T>(rect.x + rect.width, rect.y)))
					return true;
				if (contains(Vector2<T>(rect.x, rect.y + rect.height)))
					return true;
				if (contains(Vector2<T>(rect.x + rect.width, rect.y + rect.height)))
					return true;
				// One of the corners of this rectangle is in rect?
				if (rect.contains(Vector2<T>(x, y)))
					return true;
				if (rect.contains(Vector2<T>(x + width, y)))
					return true;
				if (rect.contains(Vector2<T>(x, y + height)))
					return true;
				if (rect.contains(Vector2<T>(x + width, y + height)))
					return true;
				return false;
			}

			/**
			 * X coordinate of the upper left point of the rectangle.
			 */
			T x;
			/**
			 * Y coordinate of the upper left point of the rectangle.
			 */
			T y;
			/**
			 * Width of the rectangle.
			 */
			T width;
			/**
			 * Height of the rectangle.
			 */
			T height;
	};

	typedef Rectangle<int> RectangleI;
	typedef Rectangle<float> RectangleF;
}

#endif
