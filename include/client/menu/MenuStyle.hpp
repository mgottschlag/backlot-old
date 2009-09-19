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

#ifndef _MENUSTYLE_HPP_
#define _MENUSTYLE_HPP_

#include "ReferenceCounted.hpp"

#include <string>

class TiXmlElement;

namespace backlot
{
	/**
	 * Button style.
	 */
	enum ButtonStyle
	{
		/**
		 * Normal push button style, grey box with black text.
		 */
		EBS_Normal,
		/**
		 * Simple button style: Only text which gets coloured differently
		 * depending on the state of the button.
		 */
		EBS_Simple
	};

	/**
	 * Class defining the look of a menu or dialog.
	 */
	class MenuStyle : public ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 */
			MenuStyle();
			/**
			 * Destructor.
			 */
			~MenuStyle();

			/**
			 * Loads the style from an xml file.
			 */
			bool load(TiXmlElement *xml);

			/**
			 * Returns the name of the style.
			 */
			std::string getName();

			/**
			 * Sets the button style.
			 */
			void setButtonStyle(ButtonStyle style);
			/**
			 * Returns the button style.
			 */
			ButtonStyle getButtonStyle();

			/**
			 * Returns the background color.
			 */
			unsigned int getBackgroundColor();
			/**
			 * Returns the text color.
			 */
			unsigned int getTextColor();
			/**
			 * Returns the color for currently activated elements.
			 */
			unsigned int getActiveColor();
			/**
			 * Returns the color for pressed-down buttons.
			 */
			unsigned int getPressedColor();

			/**
			 * Converts a RGB color in hexadecimal notation (#RRGGBB) into an
			 * unsigned integer.
			 */
			static unsigned int toColor(std::string str);
		private:
			std::string name;

			ButtonStyle buttonstyle;

			unsigned int bgcolor;
			unsigned int textcolor;
			unsigned int activecolor;
			unsigned int pressedcolor;
	};

	typedef SharedPointer<MenuStyle> MenuStylePointer;
}

#endif
