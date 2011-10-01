#pragma once

#include <QtGui/QDialog>

#include "ui_filterdialog.h"

class PlaceModel;
class ItemTypeModel;

class FilterDialog : public QDialog {
	Q_OBJECT
	Q_DISABLE_COPY(FilterDialog)
public:
	FilterDialog(QWidget *parent = 0);
	virtual ~FilterDialog();

	// @todo: struct for filter
	// @todo: get old filter, make filter strcut app-wide and static.
	bool usesPlace() const;
	bool usesItemType() const;
	bool usesActivity() const;
	int placeId() const;
	int itemTypeId() const;
	bool activity() const;
private:
	Ui::FilterDialog ui;
	PlaceModel *placeModel;
	ItemTypeModel *itemTypeModel;
};
