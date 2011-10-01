#include <QtGui/QToolBar>
#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>

#include "models.h"
#include "ui_addlistdialog.h"
#include "listdialog.h"

// @todo - windows instead of dialogs, but modal
ListDialog::ListDialog(bool placeMode, QWidget *parent) : QDialog(parent) {
	this->placeMode = placeMode;

	ui.setupUi(this);
	connect(ui.actionAdd, SIGNAL(triggered()), this, SLOT(addItem()));
	connect(ui.actionAddList, SIGNAL(triggered()), this, SLOT(addList()));

	QToolBar *toolBar = new QToolBar(tr("Tool bar"));
	toolBar->addAction(ui.actionAdd);
	toolBar->addAction(ui.actionAddList);
	QBoxLayout *box = static_cast<QBoxLayout*>(this->layout());
	if(box)
		box->insertWidget(0, toolBar);

	if(placeMode)
		model = new PlaceModel(this);
	else
		model = new ItemTypeModel(this);

	ui.view->setModel(model);
}

ListDialog::~ListDialog() {

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

