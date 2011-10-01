#pragma once

#include <QtGui/QDialog>

#include "inventory.h"
#include "ui_historydialog.h"

class HistoryModel;

class HistoryDialog : public QDialog {
	Q_OBJECT
	Q_DISABLE_COPY(HistoryDialog)
public:
	HistoryDialog(const Item &item, QWidget *parent = 0);
	virtual ~HistoryDialog();

private:
	Ui::HistoryDialog ui;
	HistoryModel *model;
};
