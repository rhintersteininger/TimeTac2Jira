#pragma once

#include <QDialog>
#include "ui_TicketChooserDialog.h"
#include <JiraData.h>
#include "TimeTableEntryModel.h"

class TicketChooserDialog : public QDialog
{
	Q_OBJECT

public:
	TicketChooserDialog(TimeTac::TimeTableItemModel item_, std::vector<Jira::Data::GetIssue> issues_, QWidget* parent = Q_NULLPTR);
	~TicketChooserDialog();

	std::vector<Jira::Data::GetIssue> get_selected_issues();

private:
	Ui::TicketChooserDialog ui;

	std::vector<Jira::Data::GetIssue> _issues;
};
