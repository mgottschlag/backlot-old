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

EditorWindow::EditorWindow() : QMainWindow(), newmap(this), editgroup(this)
{
	ui.setupUi(this);

	// Set up input
	QObject::connect(ui.ActionNew, SIGNAL(activated()), &newmap, SLOT(exec()));
	QObject::connect(ui.ActionOpen, SIGNAL(activated()), this, SLOT(open()));
	QObject::connect(ui.ActionSave, SIGNAL(activated()), this, SLOT(save()));
	QObject::connect(ui.ActionSaveAs, SIGNAL(activated()), this, SLOT(saveAs()));
	QObject::connect(ui.ActionCompile, SIGNAL(activated()), this, SLOT(compile()));
	QObject::connect(ui.ActionAbout, SIGNAL(activated()), this, SLOT(about()));
	QObject::connect(&newmap, SIGNAL(accepted()), this, SLOT(newMap()));
	QObject::connect(ui.ActionGrid, SIGNAL(toggled(bool)), ui.levelview, SLOT(showGrid(bool)));
	ui.ActionGrid->setChecked(true);
	QObject::connect(ui.ActionMiniMap, SIGNAL(toggled(bool)), ui.levelview, SLOT(showMiniMap(bool)));
	ui.ActionMiniMap->setChecked(true);
	QObject::connect(ui.resize, SIGNAL(clicked()), this, SLOT(resize()));
	// Create edit input group
	editgroup.addAction(ui.ActionDraw);
	editgroup.addAction(ui.ActionErase);
	editgroup.addAction(ui.ActionSelect);
	editgroup.setExclusive(true);
	ui.ActionSelect->setChecked(true);
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
	ui.width->setValue(width);
	ui.height->setValue(height);
	setWindowTitle((std::string("Backlot Map Editor - ") + mapname).c_str());
	// Reset camera position
	// TODO
	// Repaint
	ui.levelview->repaint();
}
void EditorWindow::open()
{
	QMessageBox::information(this, "Unimplemented", "Currently unimplemented.");
}
void EditorWindow::save()
{
	// Save map
	if (Map::get().isLoaded())
		Map::get().save();
}
void EditorWindow::saveAs()
{
	QMessageBox::information(this, "Unimplemented", "Currently unimplemented.");
}
void EditorWindow::compile()
{
	QMessageBox::information(this, "Unimplemented", "Currently unimplemented.");
}
void EditorWindow::about()
{
	QMessageBox::about(this, "Backlot Map Editor", "Backlot Map Editor");
}
void EditorWindow::resize()
{
	if (!Map::get().isLoaded())
		return;
	Map::get().setWidth(ui.width->value());
	Map::get().setHeight(ui.height->value());
	ui.levelview->repaint();
}

void EditorWindow::closeEvent(QCloseEvent *event)
{
}

