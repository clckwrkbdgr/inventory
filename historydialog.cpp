#include "models.h"
#include "historydialog.h"

HistoryDialog::HistoryDialog(const Item &item, QWidget *parent) : QDialog(parent) {

	ui.setupUi(this);

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
}

HistoryDialog::~HistoryDialog() {

}

