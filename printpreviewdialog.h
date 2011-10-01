#pragma once

#include <QtGui/QDialog>

#include "ui_printpreviewdialog.h"

class QAbstractItemModel;

class PrintPreviewDialog : public QDialog {
	Q_OBJECT
	Q_DISABLE_COPY(PrintPreviewDialog)
public:
	PrintPreviewDialog(QWidget *parent = 0);
	virtual ~PrintPreviewDialog();

public slots:
	void print();

private:
	Ui::PrintPreviewDialog ui;
	QAbstractItemModel *model;
};

