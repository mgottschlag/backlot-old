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

#include "OpenMapDialog.hpp"

#include <QMessageBox>

OpenMapDialog::OpenMapDialog(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);
	QObject::connect(ui.maplist, SIGNAL(clicked(QModelIndex)), this, SLOT(select(QModelIndex)));
}

void OpenMapDialog::setupList(std::vector<std::string> maps)
{
	maplist.clear();
	for (unsigned int i = 0; i < maps.size(); i++)
	{
		maplist.appendRow(new QStandardItem(maps[i].c_str()));
	}
	ui.maplist->setModel(&maplist);
	selected = "";
}
std::string OpenMapDialog::getSelected()
{
	return selected;
}

void OpenMapDialog::select(const QModelIndex &index)
{
	QStandardItem *item = maplist.itemFromIndex(index);
	selected = item->text().toAscii().constData();
}
