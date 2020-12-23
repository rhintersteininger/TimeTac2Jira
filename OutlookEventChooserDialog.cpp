#include "OutlookEventChooserDialog.h"
#include "OutlookCsvParser.h"

OutlookEventChooserDialog::OutlookEventChooserDialog(std::vector<Outlook::OutlookEntry> outlookEntries_, QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	_entries = outlookEntries_;	

	ui.tblOutlookEntries->insertColumn(0);
	ui.tblOutlookEntries->insertColumn(1);
	ui.tblOutlookEntries->insertColumn(2);
	ui.tblOutlookEntries->insertColumn(3);


	ui.tblOutlookEntries->setHorizontalHeaderItem(0, new QTableWidgetItem("Title"));
	ui.tblOutlookEntries->setHorizontalHeaderItem(1, new QTableWidgetItem("From"));
	ui.tblOutlookEntries->setHorizontalHeaderItem(2, new QTableWidgetItem("Until"));
	ui.tblOutlookEntries->setHorizontalHeaderItem(3, new QTableWidgetItem("Ticket Key"));

	ui.tblOutlookEntries->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
	ui.tblOutlookEntries->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::ResizeToContents);
	ui.tblOutlookEntries->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::ResizeToContents);
	ui.tblOutlookEntries->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeMode::Fixed);

	ui.tblOutlookEntries->setColumnWidth(3, 100);

	ui.tblOutlookEntries->verticalHeader()->hide();

	int row = 0;
	for (std::vector<Outlook::OutlookEntry>::iterator it = _entries.begin(); it != _entries.end(); ++it)
	{ 
		ui.tblOutlookEntries->insertRow(row);
		QTableWidgetItem* checkItem = new QTableWidgetItem(QString(it->get_title().c_str()));
		checkItem->setCheckState(Qt::CheckState::Checked);

		ui.tblOutlookEntries->setItem(row, 0, checkItem);
		ui.tblOutlookEntries->setItem(row, 1, new QTableWidgetItem(QString(Outlook::OutlookEntry::to_string_short(it->get_from_date()).c_str())));
		ui.tblOutlookEntries->setItem(row, 2, new QTableWidgetItem(QString(Outlook::OutlookEntry::to_string_short(it->get_until_date()).c_str())));
		ui.tblOutlookEntries->setItem(row, 3, new QTableWidgetItem(""));

		row++;
	}

	connect(ui.btnFinished, &QPushButton::clicked, this, &OutlookEventChooserDialog::accept);
}


std::vector<std::tuple<Outlook::OutlookEntry, std::string>> OutlookEventChooserDialog::get_selected_items()
{
	std::vector<std::tuple<Outlook::OutlookEntry, std::string>> entries;
	for (int i = 0; i < ui.tblOutlookEntries->rowCount(); i++)
	{
		QTableWidgetItem* checkItem = ui.tblOutlookEntries->item(i, 0);
		if (checkItem->checkState() != Qt::CheckState::Checked) continue;
		
		QTableWidgetItem* ticketItem = ui.tblOutlookEntries->item(i, 3);
		if (ticketItem->text().isEmpty()) continue;
		
		entries.push_back(std::make_tuple(_entries.at(i), ticketItem->text().toStdString()));
	}
	return entries;
}


OutlookEventChooserDialog::~OutlookEventChooserDialog()
{

}
