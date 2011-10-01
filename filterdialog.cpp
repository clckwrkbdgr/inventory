#include "models.h"

#include "filterdialog.h"

FilterDialog::FilterDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	itemTypeModel = new ItemTypeModel(this);
	placeModel = new PlaceModel(this);

	ui.listItemType->setModel(itemTypeModel);
	ui.listPlace->setModel(placeModel);
}

FilterDialog::~FilterDialog() {
}

bool FilterDialog::usesPlace() const {
	return ui.groupPlace->isChecked();
}

bool FilterDialog::usesItemType() const {
	return ui.groupItemType->isChecked();
}

bool FilterDialog::usesActivity() const {
	return ui.groupActive->isChecked();
}

int FilterDialog::placeId() const {
	return placeModel->idAt(ui.listPlace->currentIndex());
}

int FilterDialog::itemTypeId() const {
	return itemTypeModel->idAt(ui.listItemType->currentIndex());
}

bool FilterDialog::activity() const {
	return ui.buttonActive->isChecked();
}

