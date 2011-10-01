#pragma once

#include <QtGui/QDialog>

#include "inventory.h"
#include "ui_filterdialog.h"

class PlaceModel;
class ItemTypeModel;

class FilterDialog : public QDialog {
	Q_OBJECT
	Q_DISABLE_COPY(FilterDialog)
public:
	FilterDialog(QWidget *parent = 0);
	virtual ~FilterDialog();

	InventoryViewFilter filter() const;
	void setFilter(const InventoryViewFilter &newFilter);
private:
	Ui::FilterDialog ui;
	PlaceModel *placeModel;
	ItemTypeModel *itemTypeModel;
};
