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

#ifndef _EDITORWINDOW_HPP_
#define _EDITORWINDOW_HPP_

#include "ui_MainWindow.h"
#include "NewMapDialog.hpp"
#include "OpenMapDialog.hpp"

#include <QStandardItemModel>

class EditorWindow : public QMainWindow
{
	Q_OBJECT
	public:
		EditorWindow();

	public slots:
		void newMap();
		void openDialog();
		void open();
		void save();
		void saveAs();
		void compile();
		void about();
		void resize();
		void selectTile(const QModelIndex &index);
		void selectEntity(const QModelIndex &index);
		void setAction(QAction *action);
		void selectPanel(int index);

	protected:
		void closeEvent(QCloseEvent *event);
	private:
		Ui::MainWindow ui;
		NewMapDialog newmap;
		OpenMapDialog openmap;
		QActionGroup editgroup;
		QStandardItemModel tilelist;
		QStandardItemModel entitylist;
};

#endif
