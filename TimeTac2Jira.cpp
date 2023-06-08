#include "TimeTac2Jira.h"
#include "TimeTacCsvParser.h"
#include "OutlookCsvParser.h"
#include "WorklogBookingTask.h"
#include "WorklogGetAssociatedIssuesTask.h"
#include "OutlookEventChooserDialog.h"

#include <sstream>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <QtConcurrent/qtconcurrentrun.h>
#include <QtConcurrent/qtconcurrent_global.h>
#include <qthread.h>
#include <qfuturewatcher.h>

Q_DECLARE_METATYPE(Jira::Data::SearchResults)
Q_DECLARE_OPAQUE_POINTER(Jira::Data::SearchResults*)

TimeTac2Jira::TimeTac2Jira(QWidget* parent)
	: QMainWindow(parent)
{

	ui.setupUi(this);
	setup_ui();
	bind_signal_slots();
}

void TimeTac2Jira::setup_ui()
{
#ifdef _DEBUG
	ui.txtJiraServer->setText("");
	ui.txtJiraUsername->setText("");
	ui.txtJiraPassword->setText("");

	_loadedTimeTacCsvFileName = "C:\\temp\\timeTac.csv";
	ui.lblLoadedCsvFile->setText(_loadedTimeTacCsvFileName);
	_loadedOutlookCsvFileName = "C:\\temp\\Calender.csv";
	ui.lblLoadedOutlookCsvFile->setText(_loadedOutlookCsvFileName);
	
#endif

	ui.tblTimeTable->setModel(&_timeTableEntryTableModel);
	ui.tblTimeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
	ui.tblTimeTable->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tblTimeTable->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	ui.tblTimeTable->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
}

void TimeTac2Jira::bind_signal_slots()
{
	connect(ui.btnLoadCsvFile, &QPushButton::clicked, this, &TimeTac2Jira::load_timetac_csv_file);
	connect(ui.btnLoadData, &QPushButton::clicked, this, &TimeTac2Jira::load_csv_data);
	connect(ui.btnBook, &QPushButton::clicked, this, &TimeTac2Jira::book_worklog);
	connect(ui.tblTimeTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(custom_context_menu_requested(QPoint)));
	connect(ui.checkAutoSearchTickets, &QCheckBox::stateChanged, this, &TimeTac2Jira::use_auto_ticket_search_changed);
	connect(ui.btnLoadOutlookCsvFile, &QPushButton::clicked, this, &TimeTac2Jira::load_outlook_csv_file);
	connect(ui.btnTestConnection, &QPushButton::clicked, this, &TimeTac2Jira::test_jira_connection);
}

void TimeTac2Jira::test_jira_connection(bool checked)
{
	if (_jiraClient == nullptr)
		_jiraClient = std::make_shared<Jira::JiraHttpClient>(ui.txtJiraUsername->text().toStdString(), ui.txtJiraPassword->text().toStdString(), ui.txtJiraServer->text().toStdString(), 443);

	try
	{
		auto currentUser = _jiraClient->get_current_user();
		if (currentUser.get_account_id() != nullptr)
		{
			QMessageBox::information(this, "TestConnecteion successfull", std::string("Test Successfull UserId: " + *currentUser.get_account_id()).c_str(), QMessageBox::Ok);
		}
	}
	catch (std::exception const& ex)
	{
		QMessageBox::information(this, "TestConnection failed", ex.what(), QMessageBox::Ok);
	}
}


void TimeTac2Jira::load_outlook_csv_file(bool checked)
{
	_loadedOutlookCsvFileName = QFileDialog::getOpenFileName(this, "Open Outlook Csv File", "", "Csv Files (*.csv *.txt)");
	ui.lblLoadedOutlookCsvFile->setText(_loadedOutlookCsvFileName);
}

void TimeTac2Jira::load_timetac_csv_file(bool checked)
{
	_loadedTimeTacCsvFileName = QFileDialog::getOpenFileName(this, "Open TimeTac Csv File", "", "Csv Files (*.csv *.txt)");
	ui.lblLoadedCsvFile->setText(_loadedTimeTacCsvFileName);
}

void TimeTac2Jira::load_csv_data()
{
	if (_jiraClient == nullptr)
		_jiraClient = std::make_shared<Jira::JiraHttpClient>(ui.txtJiraUsername->text().toStdString(), ui.txtJiraPassword->text().toStdString(), ui.txtJiraServer->text().toStdString(), 443);

	_timeTableEntryTableModel.set_jira_client(_jiraClient.get());


	if (_loadedTimeTacCsvFileName.isEmpty())
	{
		QMessageBox msgBox;
		msgBox.setText("Please load Csv File first");
		msgBox.exec();
		return;
	}

	std::vector<TimeTac::TimeTableEntry> timeTableEntries = TimeTac::TimeTableCsvParser::parse(_loadedTimeTacCsvFileName.toStdString());
	std::vector<std::tuple<tm, tm>> bookings;

	for (std::vector<TimeTac::TimeTableEntry>::iterator it = timeTableEntries.begin(); it != timeTableEntries.end(); ++it)
	{
		tm start = it->get_come_date();
		std::vector<std::tuple<tm, tm>> pauses = it->get_pause_dates();
		for (std::vector<std::tuple<tm, tm>>::iterator itPauses = pauses.begin(); itPauses != pauses.end(); ++itPauses)
		{
			tm end = std::get<0>(*itPauses);
			bookings.push_back(std::make_tuple(start, end));
			start = std::get<1>(*itPauses);
		}
		tm end = it->get_go_date();
		bookings.push_back(std::make_tuple(start, end));
	}

	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
	std::string currentDateTime(buffer);

	std::vector<TimeTac::TimeTableItemModel> items;
	int cnt = 0;
	for (std::vector<std::tuple<tm, tm>>::iterator it = bookings.begin(); it != bookings.end(); ++it)
	{
		time_t begin = mktime(&std::get<0>(*it));
		time_t end = mktime(&std::get<1>(*it));
		if (end - begin == 0)
			continue;

		TimeTac::TimeTableItemModel item = TimeTac::TimeTableItemModel(cnt++, std::get<0>(*it), std::get<1>(*it), ui.txtDefaultTicket->text());
		items.push_back(item);
	}

	_timeTableEntryTableModel.set_items(items);
	ui.tblTimeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
	ui.tblTimeTable->horizontalHeader()->setSectionResizeMode(TimeTac::TimeTableEntryTableModel::Columns::Enabled, QHeaderView::ResizeMode::ResizeToContents);

	if (!_loadedOutlookCsvFileName.isEmpty())
	{
		prefill_data_with_events();
	}

	if (ui.checkAutoSearchTickets->isChecked())
	{
		//Disable booking while fetching tickets
		ui.btnBook->setEnabled(false);
		WorklogGetAssociatedIssuesTask* getAssociatedIssueTask = new WorklogGetAssociatedIssuesTask(ui.txtJiraUsername->text().toStdString(), _jiraClient, _timeTableEntryTableModel.get_items());
		connect(getAssociatedIssueTask, &WorklogGetAssociatedIssuesTask::worklog_status_changed, &_timeTableEntryTableModel, &TimeTac::TimeTableEntryTableModel::status_changed, Qt::QueuedConnection);
		connect(getAssociatedIssueTask, &WorklogGetAssociatedIssuesTask::get_associated_issues_task_finished, &_timeTableEntryTableModel, &TimeTac::TimeTableEntryTableModel::get_associated_issues_finished, Qt::BlockingQueuedConnection);
		connect(getAssociatedIssueTask, &WorklogGetAssociatedIssuesTask::finished, this, &TimeTac2Jira::fetch_associated_issues_finished, Qt::BlockingQueuedConnection);
		QThreadPool::globalInstance()->start(getAssociatedIssueTask);
	}
}

void TimeTac2Jira::prefill_data_with_events()
{
	std::vector<Outlook::OutlookEntry> outlookEntries = Outlook::OutlookCsvParser::parse(_loadedOutlookCsvFileName.toStdString());

	OutlookEventChooserDialog eventChooser = OutlookEventChooserDialog(outlookEntries, this);

	if (eventChooser.exec())
	{
		std::vector<std::tuple<Outlook::OutlookEntry, std::string>> outlookTickets = eventChooser.get_selected_items();
		std::vector<TimeTac::TimeTableItemModel> items = _timeTableEntryTableModel.get_items();

		for (std::vector<TimeTac::TimeTableItemModel>::iterator it = items.begin(); it != items.end(); ++it)
		{
			for (std::vector<std::tuple<Outlook::OutlookEntry, std::string>>::iterator itTick = outlookTickets.begin(); itTick != outlookTickets.end(); ++itTick)
			{
				time_t timeTacFrom = std::mktime(&it->_from);
				time_t timeTacUntil = std::mktime(&it->_until);

				tm eventFromTm = tm(std::get<0>(*itTick).get_from_date());
				time_t eventFrom = std::mktime(&eventFromTm);
				tm eventUntilTm = tm(std::get<0>(*itTick).get_until_date());
				time_t eventUntil = std::mktime(&eventUntilTm);

				if (timeTacFrom < eventFrom && timeTacUntil > eventFrom)
				{
					TimeTac::TimeTableItemModel newItem = _timeTableEntryTableModel.split_item(it->_id, eventFromTm.tm_hour, eventFromTm.tm_min);

					time_t newItemTimeTacUntil = std::mktime(&newItem._until);

					if (eventUntil < newItemTimeTacUntil)
					{
						_timeTableEntryTableModel.split_item(newItem._id, eventUntilTm.tm_hour, eventUntilTm.tm_min);
					}

					_timeTableEntryTableModel.set_ticket_key(newItem, std::get<1>(*itTick));
				}
			}
		}

	}
}

void TimeTac2Jira::fetch_associated_issues_finished()
{
	ui.btnBook->setEnabled(true);
}

void TimeTac2Jira::book_worklog()
{
	std::vector<TimeTac::TimeTableItemModel> items = _timeTableEntryTableModel.get_items();

	if (_jiraClient == nullptr)
		_jiraClient = std::make_shared<Jira::JiraHttpClient>(ui.txtJiraUsername->text().toStdString(), ui.txtJiraPassword->text().toStdString(), ui.txtJiraServer->text().toStdString(), 443);

	WorklogBookingTask* bookingTask = new WorklogBookingTask(_jiraClient, items);
	connect(bookingTask, &WorklogBookingTask::worklog_status_changed, &_timeTableEntryTableModel, &TimeTac::TimeTableEntryTableModel::status_changed, Qt::QueuedConnection);

	//QSignalSpy spy(bookingTask, &WorklogBookingTask::worklog_status_changed);
	QThreadPool::globalInstance()->start(bookingTask);
}

void TimeTac2Jira::custom_context_menu_requested(QPoint point_)
{
	QMenu* menu = new QMenu(this);
	QAction* action = new QAction(QACTION_SPLIT_TIMERANGE, this);
	action->setData(point_);
	menu->addAction(action);
	menu->popup(ui.tblTimeTable->viewport()->mapToGlobal(point_));
	connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(time_table_context_menu_triggered(QAction*)));
}

void TimeTac2Jira::time_table_context_menu_triggered(QAction* action_)
{
	if (action_->text() == QACTION_SPLIT_TIMERANGE)
	{
		QPoint point = action_->data().toPoint();
		int row = ui.tblTimeTable->rowAt(point.y());
		TimeTac::TimeTableItemModel rowItem = _timeTableEntryTableModel.get_item_at(row);

		QString splitAt = QInputDialog::getText(this, "Split at", "Enter Time where to Split (hh:mm)");
		if (splitAt.isEmpty())
			return;

		int hour;
		int minute;
		sscanf_s(splitAt.toStdString().c_str(), "%d:%d", &hour, &minute);

		bool outOfRange = false;

		if (hour < rowItem._from.tm_hour || hour > rowItem._until.tm_hour)
			outOfRange = true;
		else if (hour == rowItem._from.tm_hour && minute < rowItem._from.tm_min)
			outOfRange = true;
		else if (hour == rowItem._until.tm_hour && minute > rowItem._until.tm_min)
			outOfRange = true;

		if (outOfRange)
		{
			char buffer[50];
			snprintf(&buffer[0], 50, "Enter a time between %02d:%02d and %02d:%02d", rowItem._from.tm_hour, rowItem._from.tm_min, rowItem._until.tm_hour, rowItem._until.tm_min);
			QMessageBox::warning(this, "Error", QString(buffer));
			return;
		}

		_timeTableEntryTableModel.split_item(rowItem._id, hour, minute);
	}
}

void TimeTac2Jira::use_auto_ticket_search_changed(int state_)
{
	switch (state_)
	{
	case Qt::Unchecked:
		ui.txtDefaultTicket->setEnabled(true);
		break;
	case Qt::Checked:
		ui.txtDefaultTicket->setEnabled(false);
		break;
	case Qt::PartiallyChecked:
		break;
	}

}


