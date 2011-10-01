#pragma once

#include <QtGui/QMainWindow>

#include "ui_mainwindow.h"
#include "models.h"

class QMenu;

// @todo: - some help within main window, stored in resources.
// @todo: Добавить в inventory сохранение лога в файл по двойному нажатию C-d
class MainWindow : public QMainWindow {
	Q_OBJECT
	Q_DISABLE_COPY(MainWindow)
public:
	MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

public slots:
	void addItem();
	void deactivate();
	void editPlaces();
	void editItemTypes();
	void setFilter();
	void showHistory();
	void print();

	void popupMenu(const QPoint &pos);

private slots:
	void resizeView(const QModelIndex&, const QModelIndex&);
private:
	Ui::MainWindow ui;
	QMenu *itemMenu;
	InventoryModel *model;
};
