/*
Copyright (C) 2009  Mathias Gottschlag, Simon Kerler

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

#include "menu/Dialog.hpp"
#include "Engine.hpp"
#include "Graphics.hpp"
#include "Font.hpp"
#include "menu/GuichanFont.hpp"
#include "Debug.hpp"
#include "menu/ListModel.hpp"

#include "support/tinyxml.h"

#include <iostream>
#include <guichan/widgets/window.hpp>
#include <guichan/widgets/button.hpp>
#include <guichan/widgets/tabbedarea.hpp>
#include <guichan/widgets/tab.hpp>
#include <guichan/widgets/textfield.hpp>
#include <guichan/widgets/label.hpp>
#include <guichan/widgets/checkbox.hpp>
#include <guichan/widgets/dropdown.hpp>
#include <guichan/keylistener.hpp>

namespace backlot
{
	class DialogListener : public gcn::ActionListener
	{
		public:
			DialogListener(Dialog *dialog)
			{
				this->dialog = dialog;
			}

			virtual void action(const gcn::ActionEvent &actionEvent)
			{
				dialog->injectAction(actionEvent);
			}
		private:
			Dialog *dialog;
	};

	Dialog::Dialog()
	{
		loaded = false;
		visible = false;
		listener = new DialogListener(this);
	}
	Dialog::~Dialog()
	{
		delete listener;
	}

	SharedPointer<Dialog> Dialog::get(std::string name)
	{
		// Get already loaded dialog
		std::map<std::string, DialogPointer>::iterator it = dialogs.find(name);
		if (it != dialogs.end())
		{
			return it->second;
		}
		// Load dialog
		DialogPointer dialog = new Dialog();
		if (!dialog->load(name))
		{
			return 0;
		}
		return dialog;
	}

	bool Dialog::load(std::string name)
	{
		// Open XML file
		std::string filename = Engine::get().getGameDirectory() + "/dialogs/" + name + ".xml";
		TiXmlDocument xml(filename.c_str());
		if (!xml.LoadFile() || xml.Error())
		{
			std::cerr << "Could not load XML file " << name << ".xml: " << xml.ErrorDesc() << std::endl;
			return false;
		}
		TiXmlNode *node = xml.FirstChild("dialog");
		if (!node)
		{
			std::cerr << "Parser error: <dialog> not found." << std::endl;
			return false;
		}
		TiXmlElement *root = node->ToElement();
		// Create window
		if (!root->Attribute("size") || !root->Attribute("title"))
		{
			std::cerr << "No dialog size or title given." << std::endl;
			return false;
		}
		Vector2I size = root->Attribute("size");
		std::string title = root->Attribute("title");
		window = new gcn::Window(title);
		window->setDimension(gcn::Rectangle(10, 10, size.x, size.y));
		Graphics::get().getGuichanContainer()->add(window);
		window->setVisible(false);
		// Transparency
		gcn::Color bgcolor = window->getBaseColor();
		bgcolor.a = 200;
		window->setBaseColor(bgcolor);
		// Load basic script functions
		script = new Script();
		script->addCoreFunctions();
		script->addMenuFunctions();
		script->addClientFunctions();
		script->addDialogFunctions();
		// Load widgets
		if (!parseWidgets(root, window))
		{
			return false;
		}
		// Load scripts from XML
		TiXmlNode *scriptnode = root->FirstChild("script");
		while (scriptnode)
		{
			TiXmlElement *scriptdata = scriptnode->ToElement();
			std::string code;
			if (scriptdata)
			{
				TiXmlNode *textnode = scriptdata->FirstChild();
				while (textnode)
				{
					TiXmlText *text = textnode->ToText();
					if (text && text->Value())
					{
						code += text->Value();
					}
					textnode = scriptdata->IterateChildren(textnode);
				}
			}
			script->runString(code);
			scriptnode = node->IterateChildren("script", scriptnode);
		}
		// Insert dialog into loaded list
		this->name = name;
		loaded = true;
		dialogs.insert(std::pair<std::string, DialogPointer>(name, this));
		return true;
	}
	void Dialog::unload()
	{
		grab();
		// Remove from loaded dialogs
		if (loaded)
		{
			std::map<std::string, DialogPointer>::iterator it = dialogs.find(name);
			if (it != dialogs.end())
			{
				dialogs.erase(it);
			}
		}
		drop();
	}

	void Dialog::unloadAll()
	{
		// Simply delete all content of the loaded list
		dialogs.clear();
	}

	void Dialog::setVisible(bool visible)
	{
		window->setVisible(visible);
		this->visible = visible;
		script->callFunction("on_show");
	}
	bool Dialog::isVisible()
	{
		return visible;
	}

	void Dialog::injectAction(const gcn::ActionEvent &event)
	{
		std::string idstr = event.getSource()->getId();
		std::cout << "\"" << idstr << "\": action received." << std::endl;
		if (idstr == "")
			return;
		// Call event handler
		if (script->isFunction("on_button"))
		{
			script->callFunction("on_button", atoi(idstr.c_str()));
		}
	}

	bool Dialog::parseWidgets(TiXmlElement *xml, gcn::Container *parent)
	{
		// Tab controls
		TiXmlNode *tabctrlnode = xml->FirstChild("tabcontrol");
		while (tabctrlnode)
		{
			TiXmlElement *tabctrldata = tabctrlnode->ToElement();
			if (tabctrldata)
			{
				// Check attributes
				if (!tabctrldata->Attribute("size") || !tabctrldata->Attribute("position"))
				{
					std::cerr << "Tab control size or position missing." << std::endl;
					return false;
				}
				// Create tab area
				Vector2I position = tabctrldata->Attribute("position");
				Vector2I size = tabctrldata->Attribute("size");
				gcn::TabbedArea *tabarea = new gcn::TabbedArea();
				tabarea->setDimension(gcn::Rectangle(position.x, position.y,
					size.x, size.y));
				parent->add(tabarea);
				// Add tabs
				parseTabControl(tabctrldata, tabarea);
			}
			tabctrlnode = xml->IterateChildren("tabcontrol", tabctrlnode);
		}
		// Buttons
		TiXmlNode *buttonnode = xml->FirstChild("button");
		while (buttonnode)
		{
			TiXmlElement *buttondata = buttonnode->ToElement();
			if (buttondata)
			{
				// Check attributes
				if (!buttondata->Attribute("size") || !buttondata->Attribute("position"))
				{
					std::cerr << "Button size or position missing." << std::endl;
					return false;
				}
				if (!buttondata->Attribute("id") || !buttondata->Attribute("label"))
				{
					std::cerr << "Button id or label missing." << std::endl;
					return false;
				}
				// Create button
				Vector2I position = buttondata->Attribute("position");
				Vector2I size = buttondata->Attribute("size");
				int id = atoi(buttondata->Attribute("id"));
				const char *label = buttondata->Attribute("label");
				gcn::Button *button = new gcn::Button();
				button->setCaption(label);
				button->setDimension(gcn::Rectangle(position.x, position.y,
					size.x, size.y));
				char idstr[10];
				snprintf(idstr, 10, "%d", id);
				button->setId(idstr);
				parent->add(button);
				button->addActionListener(listener);
			}
			buttonnode = xml->IterateChildren("button", buttonnode);
		}
		// Textfields
		TiXmlNode *textfieldnode = xml->FirstChild("textfield");
		while (textfieldnode)
		{
			TiXmlElement *textfielddata = textfieldnode->ToElement();
			if (textfielddata)
			{
				// Check attributes
				if (!textfielddata->Attribute("size") || !textfielddata->Attribute("position"))
				{
					std::cerr << "Textfield size or position missing." << std::endl;
					return false;
				}
				// Create textfield
				Vector2I size = textfielddata->Attribute("size");
				Vector2I position = textfielddata->Attribute("position");
				gcn::TextField *textfield = new gcn::TextField();
				textfield->setDimension(gcn::Rectangle(position.x, position.y,
					size.x, size.y));
				parent->add(textfield);
				// Add to script
				if (textfielddata->Attribute("name"))
					script->setVariable(textfielddata->Attribute("name"), textfield);
			}
			textfieldnode = xml->IterateChildren("textfield", textfieldnode);
		}
		// Labels
		TiXmlNode *labelnode = xml->FirstChild("label");
		while (labelnode)
		{
			TiXmlElement *labeldata = labelnode->ToElement();
			if (labeldata)
			{
				// Check attributes
				if (!labeldata->Attribute("size") || !labeldata->Attribute("position"))
				{
					std::cerr << "Label size or position missing." << std::endl;
					return false;
				}
				if (!labeldata->Attribute("text"))
				{
					std::cerr << "Label text missing." << std::endl;
					return false;
				}
				// Create label
				Vector2I size = labeldata->Attribute("size");
				Vector2I position = labeldata->Attribute("position");
				const char *text = labeldata->Attribute("text");
				gcn::Label *label = new gcn::Label();
				label->setCaption(text);
				label->setDimension(gcn::Rectangle(position.x, position.y,
					size.x, size.y));
				parent->add(label);
			}
			labelnode = xml->IterateChildren("label", labelnode);
		}
		// Checkboxes
		TiXmlNode *checkboxnode = xml->FirstChild("checkbox");
		while (checkboxnode)
		{
			TiXmlElement *checkboxdata = checkboxnode->ToElement();
			if (checkboxdata)
			{
				// Check attributes
				if (!checkboxdata->Attribute("size") || !checkboxdata->Attribute("position"))
				{
					std::cerr << "Checkbox size or position missing." << std::endl;
					return false;
				}
				if (!checkboxdata->Attribute("label"))
				{
					std::cerr << "Checkbox label missing." << std::endl;
					return false;
				}
				// Create checkbox
				Vector2I size = checkboxdata->Attribute("size");
				Vector2I position = checkboxdata->Attribute("position");
				const char *label = checkboxdata->Attribute("label");
				gcn::CheckBox *checkbox = new gcn::CheckBox();
				checkbox->setCaption(label);
				checkbox->setDimension(gcn::Rectangle(position.x, position.y,
					size.x, size.y));
				parent->add(checkbox);
				// Add to script
				if (checkboxdata->Attribute("name"))
					script->setVariable(checkboxdata->Attribute("name"), checkbox);
			}
			checkboxnode = xml->IterateChildren("checkbox", checkboxnode);
		}
		// Drop down menus
		TiXmlNode *dropdownnode = xml->FirstChild("dropdown");
		while (dropdownnode)
		{
			TiXmlElement *dropdowndata = dropdownnode->ToElement();
			// Check attributes
			if (!dropdowndata->Attribute("size") || !dropdowndata->Attribute("position"))
			{
				std::cerr << "Drop down size or position missing." << std::endl;
				return false;
			}
			// Parse the elements for the drop down menu
			TiXmlNode *elementnode = dropdownnode->FirstChild("element");
			TiXmlElement *elementdata;
			ListModel *listmodel = new ListModel();
			while (elementnode)
			{
				elementdata = elementnode->ToElement();
				if (elementdata->Attribute("text"))
					listmodel->addElement(elementdata->Attribute("text"));
				else
				{
					std::cerr << "Element text missing." << std::endl;
					return false;
				}
				elementnode = dropdownnode->IterateChildren("element", elementnode);
			}
			// Create dorp down menu
			Vector2I size = dropdowndata->Attribute("size");
			Vector2I position = dropdowndata->Attribute("position");
			gcn::DropDown *dropdown = new gcn::DropDown(listmodel);
			dropdown->setDimension(gcn::Rectangle(position.x, position.y,
				size.x, size.y));
			parent->add(dropdown);
			// Add script
			if (dropdowndata->Attribute("name"))
				script->setVariable(dropdowndata->Attribute("name"), dropdown);
			dropdownnode = xml->IterateChildren("dropdown", dropdownnode);
		}

		return true;
	}
	bool Dialog::parseTabControl(TiXmlElement *xml, gcn::TabbedArea *tabctrl)
	{
		// Tabs
		TiXmlNode *tabnode = xml->FirstChild("tab");
		while (tabnode)
		{
			TiXmlElement *tabdata = tabnode->ToElement();
			if (tabdata)
			{
				// Check attributes
				if (!tabdata->Attribute("label"))
				{
					std::cerr << "Tab label missing." << std::endl;
					return false;
				}
				// Create tab
				int index = -1;
				tabdata->Attribute("index", &index);
				const char *label = tabdata->Attribute("label");
				gcn::Tab *tab = new gcn::Tab();
				tab->setCaption(label);
				gcn::Container *container = new gcn::Container();
 				container->setSize(tabctrl->getWidth(), tabctrl->getHeight());
				tabctrl->addTab(tab, container);
				parseWidgets(tabdata, container);
			}
			tabnode = xml->IterateChildren("tab", tabnode);
		}
		return true;
	}

	std::map<std::string, SharedPointer<Dialog> > Dialog::dialogs;
}
