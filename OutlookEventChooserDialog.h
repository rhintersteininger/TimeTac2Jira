#pragma once

#include <QDialog>
#include "ui_OutlookEventChooserDialog.h"
#include "OutlookCsvParser.h"

class OutlookEventChooserDialog : public QDialog
{
	Q_OBJECT

public:
	OutlookEventChooserDialog(std::vector<Outlook::OutlookEntry> entries_, QWidget *parent = Q_NULLPTR);
	~OutlookEventChooserDialog();

	std::vector<std::tuple<Outlook::OutlookEntry, std::string>> get_selected_items();

private:
	Ui::OutlookEventChooserDialog ui;

	std::vector<Outlook::OutlookEntry> _entries; 
};
