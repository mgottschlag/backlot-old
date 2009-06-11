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

#ifndef _LINE_HPP_
#define _LINE_HPP_

#include "Vector2.hpp"

#include <math.h>

namespace backlot
{
	inline float max(float a, float b)
	{
		return (a > b) ? a : b;
	}
	inline float min(float a, float b)
	{
		return (a < b) ? a : b;
	}
	/**
	 * Utility class for a line between two points.
	 */
	class Line
	{
		public:
			/**
			* Constructor.
			*/
			Line() : start(0, 0), end(0, 0)
			{
			}
			/**
			* Constructor.
			*/
			Line(const Vector2F &start, const Vector2F &end) : start(start),
				end(end)
			{
			}
			/**
			* Destructor.
			*/
			~Line()
			{
			}

			/**
			* Returns the linear equation for the line in the form ax + by = c.
			*/
			void getEquation(float &a, float &b, float &c) const
			{
				if (start == end)
				{
					// Not a line
					a = 0;
					b = 0;
					c = 0;
				}
				else if (start.x != end.x)
				{
					// f(x) = mx + n
					float m = (end.y - start.y) / (end.x - start.x);
					float n = start.y - start.x * m;
					a = -m;
					b = 1;
					c = n;
				}
				else
				{
					// f(y) = my + n
					float m = (end.x - start.x) / (end.y - start.y);
					float n = start.x - start.y * m;
					a = 1;
					b = -m;
					c = n;
				}
			}
			/**
			* Computes the intersection between this line and another.
			* @param other Line to intersect this line with.
			* @param intersection Will be filled with the intersection position.
			* @return True, if the lines intersect.
			*/
			bool getIntersection(const Line &other, Vector2F &intersection) const
			{
				// Linear equations:
				// ax + by = 1
				// cx + dy = 1
				float a, b, c, d, e, f;
				getEquation(a, b, c);
				other.getEquation(d, e, f);
				// Determinants
				float det = a * e - b * d;
				// TODO: parallel
				if (det == 0)
					return false;
				float det_x = c * e - b * f;
				float det_y = a * f - c * d;
				float x = det_x / det;
				float y = det_y / det;
				// Check whether intersection is still on both lines
				if (min(start.x, end.x) - x <= 0.001
					&& max(start.x, end.x) - x >= -0.001
					&& min(start.y, end.y) - y <= 0.001
					&& max(start.y, end.y) - y >= -0.001
					&& min(other.start.x, other.end.x) - x <= 0.001
					&& max(other.start.x, other.end.x) - x >= -0.001
					&& min(other.start.y, other.end.y) - y <= 0.001
					&& max(other.start.y, other.end.y) - y >= -0.001)
				{
					intersection.x = det_x / det;
					intersection.y = det_y / det;
					return true;
				}
				else
					return false;
			}

			/**
			* Start point of the line.
			*/
			Vector2F start;
			/**
			* End point of the line.
			*/
			Vector2F end;
	};
}

#endif
