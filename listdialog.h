#pragma once

#include <QtGui/QDialog>

#include "ui_listdialog.h"

class QAbstractItemModel;

class ListDialog : public QDialog {
	Q_OBJECT
	Q_DISABLE_COPY(ListDialog)
public:
	ListDialog(bool placeMode, QWidget *parent = 0);
	virtual ~ListDialog();

public slots:
	void addItem();
	void addList();

private:
	Ui::ListDialog ui;
	bool placeMode; // @todo: normal enum for place mode in list dialog.
	QAbstractItemModel *model;
};
