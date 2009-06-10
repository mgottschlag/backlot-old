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

#ifndef _DIALOG_HPP_
#define _DIALOG_HPP_

#include "ReferenceCounted.hpp"
#include "Script.hpp"

#include <string>
#include <map>

class TiXmlElement;
namespace gcn
{
	class Window;
	class Container;
	class TabbedArea;
}

namespace backlot
{
	/**
	 * Menu dialog class.
	 */
	class Dialog : public ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 */
			Dialog();
			/**
			 * Destructor.
			 */
			~Dialog();

			/**
			 * Loads the dialog with the given name or returns an already loaded
			 * one.
			 */
			static SharedPointer<Dialog> get(std::string name);

			/**
			 * Loads a dialog.
			 */
			bool load(std::string name);
			/**
			 * Deletes a dialog from the list of loaded dialogs. As soon as no
			 * references are left, the dialog is deleted.
			 */
			void unload();

			/**
			 * Unloads all dialogs.
			 */
			static void unloadAll();

			/**
			 * Shows/hides the dialog.
			 */
			void setVisible(bool visible);
			/**
			 * Returns whether the dialog is shown.
			 */
			bool isVisible();
		private:
			bool parseWidgets(TiXmlElement *xml, gcn::Container *parent);
			bool parseTabControl(TiXmlElement *xml, gcn::TabbedArea *tabctrl);

			gcn::Window *window;
			std::string name;
			bool loaded;
			bool visible;
			ScriptPointer script;

			static std::map<std::string, SharedPointer<Dialog> > dialogs;
	};

	typedef SharedPointer<Dialog> DialogPointer;
}

#endif
