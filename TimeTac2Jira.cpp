#include "TimeTac2Jira.h"
#include "TimeTacCsvParser.h"
#include "WorklogBookingTask.h"


#include <qfiledialog.h>
#include <qmessagebox.h>
//#include <qsignalspy.h>


TimeTac2Jira::TimeTac2Jira(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    setup_ui();
    bind_signal_slots();
} 

void TimeTac2Jira::setup_ui()
{
#ifdef _DEBUG
    ui.txtJiraServer->setText("dev-rh.atlassian.net");
    ui.txtJiraUsername->setText("dev.rhintersteininger@gmail.com");
    ui.txtJiraPassword->setText("bHpyPhj3OSOgesa99zrM468D");
	_loadedFileName = "C:\\temp\\timeTac.csv";
	ui.lblLoadedCsvFile->setText(_loadedFileName);
#endif
    ui.tblTimeTable->setModel(&_timeTableEntryTableModel);

	ui.tblTimeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch); 
}

void TimeTac2Jira::bind_signal_slots()
{
    connect(ui.btnLoadCsvFile, &QPushButton::clicked, this, &TimeTac2Jira::load_timetac_csv_file);
    connect(ui.btnLoadData, &QPushButton::clicked, this, &TimeTac2Jira::load_csv_data);
	connect(ui.btnBook, &QPushButton::clicked, this, &TimeTac2Jira::book_worklog);
}

void TimeTac2Jira::load_timetac_csv_file(bool checked)
{
   _loadedFileName = QFileDialog::getOpenFileName(this, "Open TimeTac Csv File", "", "Csv Files (*.csv *.txt)");
   ui.lblLoadedCsvFile->setText(_loadedFileName);
}

void TimeTac2Jira::load_csv_data()
{
    if (_loadedFileName.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setText("Please load Csv File first");
        msgBox.exec();
        return;
    }

    std::vector<TimeTac::TimeTableEntry> timeTableEntries = TimeTac::TimeTableCsvParser::parse(_loadedFileName.toStdString());
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

}

void TimeTac2Jira::book_worklog()
{
	std::vector<TimeTac::TimeTableItemModel> items = _timeTableEntryTableModel.get_items();

	if (_jiraClient == nullptr)
		_jiraClient = std::make_shared<Jira::JiraHttpClient>(ui.txtJiraUsername->text().toStdString(), ui.txtJiraPassword->text().toStdString(), ui.txtJiraServer->text().toStdString(), 443);

	WorklogBookingTask* bookingTask = new WorklogBookingTask(_jiraClient, items);
	auto connection = connect(bookingTask, &WorklogBookingTask::worklog_status_changed, &_timeTableEntryTableModel, &TimeTac::TimeTableEntryTableModel::status_changed, Qt::QueuedConnection);

	//QSignalSpy spy(bookingTask, &WorklogBookingTask::worklog_status_changed);
	QThreadPool::globalInstance()->start(bookingTask);
}

