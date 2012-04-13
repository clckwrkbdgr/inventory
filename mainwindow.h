#pragma once

#include <QtGui/QMainWindow>

#include "models.h"

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow {
	Q_OBJECT;
	Q_DISABLE_COPY(MainWindow);
public:
	MainWindow(QWidget * parent = 0);
	virtual ~MainWindow();
private:
	Ui::MainWindow ui;
};
