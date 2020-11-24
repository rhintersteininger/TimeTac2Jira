#include "TicketChooserDialog.h"

TicketChooserDialog::TicketChooserDialog(TimeTac::TimeTableItemModel item_, std::vector<Jira::Data::GetIssue> issues_, QWidget *parent)
	: QDialog(parent), _issues(issues_)
{
	ui.setupUi(this);

	std::string text = std::string("Choose your Tickets for the Timerange: ");
	ui._lblTitleDialog->setText(QString((text + TimeTac::TimeTableItemModel::to_jira_string_short(&item_._from) + " - " + TimeTac::TimeTableItemModel::to_time(&item_._until)).c_str()));

	ui._tblTickets->setRowCount(_issues.size());
	ui._tblTickets->setColumnCount(1);

	int row = 0;
	for(std::vector<Jira::Data::GetIssue>::iterator it = _issues.begin(); it != _issues.end(); ++it)
	{
		QTableWidgetItem* item = new QTableWidgetItem(QString((*it->get_key() + " - " + *it->get_fields()->get_summary()).c_str()));
		ui._tblTickets->setItem(row, 0, item);
		row++;
	}

	connect(ui._btnCancel, &QPushButton::clicked, this, &TicketChooserDialog::reject);
	connect(ui._btnOk, &QPushButton::clicked, this, &TicketChooserDialog::accept);
}

std::vector<Jira::Data::GetIssue> TicketChooserDialog::get_selected_issues()
{
	std::vector<Jira::Data::GetIssue> selectedIssues;
	QList<QTableWidgetItem*> items = ui._tblTickets->selectedItems();
	for (QList<QTableWidgetItem*>::iterator it = items.begin(); it != items.end(); ++it)
	{
		int row = (*it)->row();
		selectedIssues.push_back(_issues.at(row));
	}
	return selectedIssues;
}


TicketChooserDialog::~TicketChooserDialog()
{
}
