#include <QtSql/QSqlQueryModel>
#include <QtGui/QPrinter>
#include <QtGui/QPrintDialog>
#include <QtGui/QPainter>
#include <QtGui/QMessageBox>
#include <QtSql/QSqlError>

#include "models.h"
#include "printpreviewdialog.h"

// @todo - windows instead of dialogs, but modal
PrintPreviewDialog::PrintPreviewDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(print()));

	try {
		model = new SortingSqlQueryModel(this);
	} catch(IIdObject::InvalidIdException e) {
		QMessageBox::critical(this, tr("Print error"), tr("Database invalid id exception!"));
	} catch(IDatabaseObject::DBErrorException e) {
		QMessageBox::critical(this, tr("Print error"), e.query.lastError().databaseText() + "\n" + e.query.lastError().driverText());
	}

	model->setHeaderData(0, Qt::Horizontal, tr("Item type"));
	model->setHeaderData(1, Qt::Horizontal, tr("Name"));
	model->setHeaderData(2, Qt::Horizontal, tr("Count"));
	model->setHeaderData(3, Qt::Horizontal, tr("INN"));
	model->setHeaderData(4, Qt::Horizontal, tr("Place"));
	model->setHeaderData(5, Qt::Horizontal, tr("Active"));

	ui.view->setModel(model);
	ui.view->resizeColumnsToContents();
}

PrintPreviewDialog::~PrintPreviewDialog() {

}

void PrintPreviewDialog::print() {
	QPrinter printer;
	QPrintDialog *dialog = new QPrintDialog(&printer, this);
	dialog->setWindowTitle(tr("Print report"));
	if(dialog->exec()) {
		// Prepare main table view for printing: resize it to the page size.
		ui.view->resize(printer.pageRect().size());
		Qt::ScrollBarPolicy horPolicy = ui.view->horizontalScrollBarPolicy(),
			verPolicy = ui.view->verticalScrollBarPolicy();
		ui.view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		ui.view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		ui.view->clearSelection();

		// Print.
		QPainter painter;
		painter.begin(&printer);

		int rowsPrinted = 0;
		ui.view->scrollTo(model->index(0, 0), QAbstractItemView::PositionAtTop);
		while(true) {
			// Render rows.
			ui.view->render(&painter);

			// Hide already printed rows and get it's count.
			int currentBottomRow = ui.view->rowAt(ui.view->rect().bottom()) - 1;
			for(int row = rowsPrinted; row < currentBottomRow; row++)
				ui.view->hideRow(row);
			rowsPrinted = currentBottomRow;
			if(rowsPrinted < 0)
				rowsPrinted = model->rowCount();

			// Set next page or quit.
			if(rowsPrinted < model->rowCount())
				printer.newPage();
			else
				break;
		}

		painter.end();

		// Resize table view back.
		ui.verticalLayout->invalidate();
		ui.view->setVerticalScrollBarPolicy(verPolicy);
		ui.view->setHorizontalScrollBarPolicy(horPolicy);
		for(int row = 0; row < model->rowCount(); row++) {
			ui.view->showRow(row);
		}
	}
	delete dialog;
}


