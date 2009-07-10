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

#include "EditorWindow.hpp"
#include "Map.hpp"

#include <QMessageBox>

EditorWindow::EditorWindow() : QMainWindow(), newmap(this)
{
	ui.setupUi(this);

	QObject::connect(ui.ActionNew, SIGNAL(activated()), &newmap, SLOT(exec()));
	QObject::connect(ui.ActionOpen, SIGNAL(activated()), this, SLOT(open()));
	QObject::connect(ui.ActionSave, SIGNAL(activated()), this, SLOT(save()));
	QObject::connect(ui.ActionSaveAs, SIGNAL(activated()), this, SLOT(saveAs()));
	QObject::connect(ui.ActionCompile, SIGNAL(activated()), this, SLOT(compile()));
	QObject::connect(ui.ActionAbout, SIGNAL(activated()), this, SLOT(about()));
	QObject::connect(&newmap, SIGNAL(accepted()), this, SLOT(newMap()));
}

void EditorWindow::newMap()
{
	// Get map info
	std::string mapname = newmap.ui.name->text().toAscii().constData();
	int width = newmap.ui.width->value();
	int height = newmap.ui.height->value();
	// Check whether a map is loaded
	if (Map::get().isLoaded())
	{
		if (QMessageBox::question(this, "Close current map", "A map is loaded. "
			"If you continue, all current changes are lost.",
			QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
			return;
	}
	// Check whether a map with that name already exists
	// TODO
	// Create map
	Map::get().create(mapname);
	Map::get().setWidth(width);
	Map::get().setHeight(height);
}
void EditorWindow::open()
{
}
void EditorWindow::save()
{
}
void EditorWindow::saveAs()
{
}
void EditorWindow::compile()
{
}
void EditorWindow::about()
{
	QMessageBox::about(this, "Backlot Map Editor", "Backlot Map Editor");
}

void EditorWindow::closeEvent(QCloseEvent *event)
{
}

