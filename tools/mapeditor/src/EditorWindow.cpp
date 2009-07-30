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
#include "TileSet.hpp"
#include "Game.hpp"

#include <QMessageBox>
#include <iostream>

EditorWindow::EditorWindow() : QMainWindow(), newmap(this), editgroup(this)
{
	ui.setupUi(this);

	// Set up input
	QObject::connect(ui.ActionNew, SIGNAL(activated()), &newmap, SLOT(exec()));
	QObject::connect(ui.ActionOpen, SIGNAL(activated()), this, SLOT(openDialog()));
	QObject::connect(ui.ActionSave, SIGNAL(activated()), this, SLOT(save()));
	QObject::connect(ui.ActionSaveAs, SIGNAL(activated()), this, SLOT(saveAs()));
	QObject::connect(ui.ActionCompile, SIGNAL(activated()), this, SLOT(compile()));
	QObject::connect(ui.ActionAbout, SIGNAL(activated()), this, SLOT(about()));
	QObject::connect(&newmap, SIGNAL(accepted()), this, SLOT(newMap()));
	QObject::connect(&openmap, SIGNAL(accepted()), this, SLOT(open()));
	QObject::connect(ui.ActionGrid, SIGNAL(toggled(bool)), ui.levelview, SLOT(showGrid(bool)));
	ui.ActionGrid->setChecked(true);
	QObject::connect(ui.ActionMiniMap, SIGNAL(toggled(bool)), ui.levelview, SLOT(showMiniMap(bool)));
	ui.ActionMiniMap->setChecked(true);
	QObject::connect(ui.resize, SIGNAL(clicked()), this, SLOT(resize()));
	QObject::connect(ui.tilelist, SIGNAL(clicked(QModelIndex)), this, SLOT(selectTile(QModelIndex)));
	QObject::connect(ui.entitylist, SIGNAL(clicked(QModelIndex)), this, SLOT(selectEntity(QModelIndex)));
	QObject::connect(ui.toolbox, SIGNAL(currentChanged(int)), this, SLOT(selectPanel(int)));
	// Create edit input group
	editgroup.addAction(ui.ActionDraw);
	editgroup.addAction(ui.ActionErase);
	editgroup.addAction(ui.ActionSelect);
	editgroup.setExclusive(true);
	ui.ActionSelect->setChecked(true);
	QObject::connect(&editgroup, SIGNAL(triggered(QAction*)), this, SLOT(setAction(QAction*)));
	// Fill tile list
	std::vector<std::string> tilenames = TileSet::getTiles();
	for (unsigned int i = 0; i < tilenames.size(); i++)
	{
		tilelist.appendRow(new QStandardItem(tilenames[i].c_str()));
	}
	ui.tilelist->setModel(&tilelist);
	// Fill entity list
	std::vector<std::string> entitynames = Game::get().getEntities();
	for (unsigned int i = 0; i < entitynames.size(); i++)
	{
		entitylist.appendRow(new QStandardItem(entitynames[i].c_str()));
	}
	ui.entitylist->setModel(&entitylist);
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
void EditorWindow::openDialog()
{
	openmap.setupList(Game::get().getMaps());
	openmap.exec();
}
void EditorWindow::open()
{
	std::string mapname = openmap.getSelected();
	if (mapname == "")
	{
		QMessageBox::critical(this, "Error", "No map selected.");
		return;
	}
	if (!Map::get().load(mapname))
	{
		QMessageBox::critical(this, "Error", "Map could not be loaded.");
	}
}
void EditorWindow::save()
{
	// Save map
	if (Map::get().isLoaded())
		if (!Map::get().save())
			QMessageBox::critical(this, "Error", "Map could not be saved.");
}
void EditorWindow::saveAs()
{
	QMessageBox::information(this, "Unimplemented", "Currently unimplemented.");
}
void EditorWindow::compile()
{
	if (Map::get().isLoaded())
		if (!Map::get().compile())
			QMessageBox::critical(this, "Error", "Map could not be compiled.");
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
void EditorWindow::selectTile(const QModelIndex &index)
{
	// Get tile
	QStandardItem *item = tilelist.itemFromIndex(index);
	std::string tilename = item->text().toAscii().constData();
	// Set tile as the current tile for editing
	ui.levelview->setTile(TileSet::getTile(tilename));
	// Update preview
	// TODO
}
void EditorWindow::selectEntity(const QModelIndex &index)
{
	// Get entity
	QStandardItem *item = entitylist.itemFromIndex(index);
	std::string entity = item->text().toAscii().constData();
	// Set entity as the current entity for editing
	ui.levelview->setEntity(entity);
}
void EditorWindow::setAction(QAction *action)
{
	if (action == ui.ActionDraw)
	{
		ui.levelview->setUserAction(EUA_Draw);
	}
	else if (action == ui.ActionErase)
	{
		ui.levelview->setUserAction(EUA_Erase);
	}
	else
	{
		ui.levelview->setUserAction(EUA_None);
	}
}
void EditorWindow::selectPanel(int index)
{
	if (ui.toolbox->widget(index) == ui.tiles)
	{
		std::cout << "Tile panel." << std::endl;
		// Set edit mode
		ui.levelview->setDrawMode(EDM_Tile);
	}
	else if (ui.toolbox->widget(index) == ui.entities)
	{
		std::cout << "Entity panel." << std::endl;
		ui.levelview->setDrawMode(EDM_Entity);
	}
	else
	{
		// Disable editing
		ui.levelview->setDrawMode(EDM_None);
	}
}

void EditorWindow::closeEvent(QCloseEvent *event)
{
}

