#include <QtGui/QMessageBox>
#include <QtSql/QSqlError>

#include "models.h"

#include "historydialog.h"

HistoryDialog::HistoryDialog(const Item &item, QWidget *parent) : QDialog(parent) {

	ui.setupUi(this);

	try {
		model = new HistoryModel(item, this);
		ui.view->setModel(model);
		ui.view->resizeColumnsToContents();

		ui.labelItem->setText(tr("%2 '%1' at %3, inn: %4%5").
				arg(item.name()).
				arg(item.itemType().name()).
				arg(item.place().name()).
				arg(item.innExists() ? QString::number(item.inn()) : tr("N/A")).
				arg(item.isActive() ? tr("", "item is active in history header") : tr(", inactive"))
		);
	} catch(IIdObject::InvalidIdException e) {
		QMessageBox::critical(this, tr("History error"), tr("Database invalid id exception!"));
	} catch(IDatabaseObject::DBErrorException e) {
		QMessageBox::critical(this, tr("History error"), e.query.lastError().databaseText() + "\n" + e.query.lastError().driverText());
	}
}

HistoryDialog::~HistoryDialog() {

}

