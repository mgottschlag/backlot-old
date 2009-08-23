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

#ifndef _INPUTRECEIVER_HPP_
#define _INPUTRECEIVER_HPP_

#include "menu/Menu.hpp"
#include "menu/Dialog.hpp"

#include <guichan/actionlistener.hpp>

namespace backlot
{
	class InputReceiver : public gcn::ActionListener
	{
		public:
			InputReceiver(Menu *menu)
			{
				this->menu = menu;
				dialog = 0;
			}
			InputReceiver(Dialog *menu)
			{
				this->dialog = dialog;
				menu = 0;
			}

			virtual void action(const gcn::ActionEvent &actionEvent)
			{
				if (menu)
					menu->injectAction(actionEvent);
				if (dialog)
					dialog->injectAction(actionEvent);
			}
		private:
			Menu *menu;
			Dialog *dialog;
	};
}

#endif
