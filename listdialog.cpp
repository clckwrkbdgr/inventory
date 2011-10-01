#include <QtGui/QToolBar>
#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>
#include <QtSql/QSqlError>

#include "models.h"
#include "ui_addlistdialog.h"
#include "listdialog.h"

// @todo - windows instead of dialogs, but modal
ListDialog::ListDialog(bool placeMode, QWidget *parent) : QDialog(parent) {
	this->placeMode = placeMode;

	ui.setupUi(this);
	connect(ui.actionAdd, SIGNAL(triggered()), this, SLOT(addItem()));
	connect(ui.actionRemove, SIGNAL(triggered()), this, SLOT(removeItem()));
	connect(ui.actionAddList, SIGNAL(triggered()), this, SLOT(addList()));

	QToolBar *toolBar = new QToolBar(tr("Tool bar"));
	toolBar->addAction(ui.actionAdd);
	toolBar->addAction(ui.actionAddList);
	toolBar->addAction(ui.actionRemove);
	QBoxLayout *box = static_cast<QBoxLayout*>(this->layout());
	if(box)
		box->insertWidget(0, toolBar);

	try {
		if(placeMode)
			model = new PlaceModel(this);
		else
			model = new ItemTypeModel(this);
	} catch(IIdObject::InvalidIdException e) {
		QMessageBox::critical(this, tr("Error"), tr("Database invalid id exception!"));
	} catch(IDatabaseObject::DBErrorException e) {
		QMessageBox::critical(this, tr("Error"), e.query.lastError().databaseText() + "\n" + e.query.lastError().driverText());
	}


	ui.view->setModel(model);
}

ListDialog::~ListDialog() {

}

void ListDialog::removeItem() {
	if(QMessageBox::question(this, tr("Remove value"), tr("Do you really want to remove selected value?"),
							QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Yes) {
		if(!model->removeRow(ui.view->currentIndex().row()))
			QMessageBox::critical(this, tr("Remove value"), tr("Cannot remove selected value.\n"
						"Probably, there is items in the inventory, that use this value."));
	}
}

void ListDialog::addItem() {
	model->insertRow(model->rowCount());
}

void ListDialog::addList() {
	QDialog *dialog = new QDialog(this);
	Ui::AddListDialog addListDialogUi;
	addListDialogUi.setupUi(dialog);
	dialog->exec();

	QStringList lines = addListDialogUi.edit->toPlainText().split('\n');
	foreach(QString line, lines)
		if(!line.isEmpty()) {
			if(placeMode)
				Place::add(line);
			else
				ItemType::add(line);
		}

	PlaceModel *pModel = static_cast<PlaceModel*>(this->model);
	ItemTypeModel *itModel = static_cast<ItemTypeModel*>(this->model);
	if(itModel)
		itModel->updateList();
	if(pModel)
		pModel->updateList();

	delete dialog;
}

