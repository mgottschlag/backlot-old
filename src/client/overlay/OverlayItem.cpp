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

#include "overlay/OverlayItem.hpp"

namespace backlot
{
	OverlayItem::OverlayItem()
	{
	}
	OverlayItem::~OverlayItem()
	{
	}
	
	void OverlayItem::setName(std::string name)
	{
		this->name = name;
	}
	std::string OverlayItem::getName()
	{
		return name;
	}
	void OverlayItem::setPosition(Vector2I position)
	{
		this->position = position;
	}
	Vector2I OverlayItem::getPosition()
	{
		return position;
	}
	void OverlayItem::setOffset(Vector2I offset)
	{
		this->offset = offset;
	}
	Vector2I OverlayItem::getOffset()
	{
		return offset;
	}
	void OverlayItem::setSize(Vector2I size)
	{
		this->size = size;
	}
	Vector2I OverlayItem::getSize()
	{
		return size;
	}
	
	bool OverlayItem::load(TiXmlNode *node)
	{
		return true;
	}
	
	void OverlayItem::render()
	{
	}
}
