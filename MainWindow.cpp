#include "MainWindow.h"
#include "TimeTacCsvParser.h"

#define DEBUG


wxBEGIN_EVENT_TABLE(TimeTac2Jira::MainWindow, wxFrame)
EVT_BUTTON(1000, TimeTac2Jira::MainWindow::LoadData)
EVT_BUTTON(1001, TimeTac2Jira::MainWindow::BookWorklog)
EVT_WORKLOG_UPDATE(wxID_ANY, TimeTac2Jira::MainWindow::WorklogUpdated)
wxEND_EVENT_TABLE()

TimeTac2Jira::MainWindow::MainWindow() : wxFrame(nullptr, wxID_ANY, "MainWindow", wxDefaultPosition, wxSize(1000, 720))
{
	_panelBase = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, "MainPanel");

#pragma region Sizer
	_boxMainContainer = new wxBoxSizer(wxVERTICAL);
	_boxSettingsContainer = new wxBoxSizer(wxHORIZONTAL);

	_flexGridJiraServerSettings = new wxFlexGridSizer(3, 2, 5, 10);
	_flexGridJiraServerSettings->AddGrowableCol(1, 1);

	_flexGridTimeTacCsvSettings = new wxFlexGridSizer(1, 2, 5, 10);
	_flexGridTimeTacCsvSettings->AddGrowableCol(1, 1);

	_flexGridSimpleTicketSelector = new wxFlexGridSizer(1, 2, 5, 10);
	_flexGridSimpleTicketSelector->AddGrowableCol(1, 1);

	_flexGridConfirmDataTable = new wxFlexGridSizer(1, 1, 5, 10);
	_flexGridConfirmDataTable->AddGrowableCol(0, 1);
	_flexGridConfirmDataTable->AddGrowableRow(0, 1);

	_boxButtonGroup = new wxBoxSizer(wxHORIZONTAL);

	_boxMainContainer->Add(_boxSettingsContainer, 0, wxEXPAND | wxALL, 10);
	_boxMainContainer->Add(_flexGridConfirmDataTable, 1, wxEXPAND | wxALIGN_TOP | wxALL, 10);
	_boxMainContainer->Add(_boxButtonGroup, 0, wxALL | wxALIGN_RIGHT, 10);
	_boxSettingsContainer->Add(_flexGridJiraServerSettings, 1, wxEXPAND | wxRIGHT, 10);
	_boxSettingsContainer->Add(_flexGridTimeTacCsvSettings, 1, wxEXPAND | wxRIGHT, 10);
	_boxSettingsContainer->Add(_flexGridSimpleTicketSelector, 1, wxEXPAND | wxRIGHT, 10);

#pragma endregion

#pragma region Input Settings
	_btnAnalyze = new wxButton(_panelBase, 1000, "Load Data");
	_lblJiraServer = new wxStaticText(_panelBase, wxID_ANY, "Server: ");
	_lblJiraUsername = new wxStaticText(_panelBase, wxID_ANY, "Username: ");
	_lblJiraPassword = new wxStaticText(_panelBase, wxID_ANY, "Password: ");

	_lblTimeTacCsv = new wxStaticText(_panelBase, wxID_ANY, "TimeTac CSV: ");

	_lblSimpleJiraTicket = new wxStaticText(_panelBase, wxID_ANY, "Ticket: ");

#ifdef DEBUG
	_txtJiraServer = new wxTextCtrl(_panelBase, wxID_ANY, "dev-rh.atlassian.net");
	_txtJiraUsername = new wxTextCtrl(_panelBase, wxID_ANY, "dev.rhintersteininger@gmail.com");
	_txtJiraPassword = new wxTextCtrl(_panelBase, wxID_ANY, "bHpyPhj3OSOgesa99zrM468D");
	_txtTimeTacCsvPath = new wxTextCtrl(_panelBase, wxID_ANY, "C:\\temp\\timeTac.csv");
	_txtSimpleJiraTicket = new wxTextCtrl(_panelBase, wxID_ANY, "TEST-3");
#else
	_txtJiraServer = new wxTextCtrl(_panelBase, wxID_ANY);
	_txtJiraUsername = new wxTextCtrl(_panelBase, wxID_ANY);
	_txtJiraPassword = new wxTextCtrl(_panelBase, wxID_ANY);
	_txtCsvPath = new wxTextCtrl(_panelBase, wxID_ANY);
	_txtSimpleJiraTicket = new wxTextCtrl(_panelBase, wxID_ANY);
#endif

	_txtJiraUsername->MoveAfterInTabOrder(_txtJiraServer);
	_txtJiraPassword->MoveAfterInTabOrder(_txtJiraUsername);
	_txtTimeTacCsvPath->MoveAfterInTabOrder(_txtJiraPassword);
	_txtSimpleJiraTicket->MoveAfterInTabOrder(_txtTimeTacCsvPath);
	_btnAnalyze->MoveAfterInTabOrder(_txtTimeTacCsvPath);

#ifdef __WXMSW__
	HWND hwnd = (HWND)_txtJiraPassword->GetHandle();
	SendMessage(hwnd, EM_SETPASSWORDCHAR, 0x25cf, 0); // 0x25cf is ● character
#else
	_txtJiraPassword->SetWindowStyle(wxTE_PASSWORD);
#endif
	_txtJiraPassword->Refresh();

	_boxSettingsContainer->Add(_btnAnalyze);

	_flexGridJiraServerSettings->Add(_lblJiraServer, 1, wxEXPAND);
	_flexGridJiraServerSettings->Add(_txtJiraServer, 1, wxEXPAND);

	_flexGridJiraServerSettings->Add(_lblJiraUsername, 1, wxEXPAND);
	_flexGridJiraServerSettings->Add(_txtJiraUsername, 1, wxEXPAND);

	_flexGridJiraServerSettings->Add(_lblJiraPassword, 1, wxEXPAND);
	_flexGridJiraServerSettings->Add(_txtJiraPassword, 1, wxEXPAND);

	_flexGridTimeTacCsvSettings->Add(_lblTimeTacCsv, 1, wxEXPAND);
	_flexGridTimeTacCsvSettings->Add(_txtTimeTacCsvPath, 1, wxEXPAND);

	_flexGridSimpleTicketSelector->Add(_lblSimpleJiraTicket, 1, wxEXPAND);
	_flexGridSimpleTicketSelector->Add(_txtSimpleJiraTicket, 1, wxEXPAND);



#pragma endregion

#pragma region Data Visualisation

	_lstViewWorklogs = new wxListView(_panelBase, wxID_ANY);

	InitListView();


	_btnConfirmBookings = new wxButton(_panelBase, 1001, "Confirm and book");
	_boxButtonGroup->Add(_btnConfirmBookings);


	_flexGridConfirmDataTable->Add(_lstViewWorklogs, 1, wxEXPAND);


#pragma endregion

	//this->SetSizerAndFit(_boxMainContainer);
	_panelBase->SetSizerAndFit(_boxMainContainer);
}

void TimeTac2Jira::MainWindow::InitListView()
{
	_lstViewWorklogs->AppendColumn("Start");
	_lstViewWorklogs->AppendColumn("Time Spent");
	_lstViewWorklogs->AppendColumn("Ticket");
	_lstViewWorklogs->AppendColumn("Status");

	_lstViewWorklogs->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
	_lstViewWorklogs->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
	_lstViewWorklogs->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
	_lstViewWorklogs->SetColumnWidth(3, wxLIST_AUTOSIZE_USEHEADER);
}

void TimeTac2Jira::MainWindow::LoadData(wxCommandEvent& event)
{
	if (_jiraClient == nullptr)
		_jiraClient = new Jira::JiraHttpClient(_txtJiraUsername->GetValue().ToStdString(), _txtJiraPassword->GetValue().ToStdString(), _txtJiraServer->GetValue().ToStdString(), 443);

	if (_currentWorklogList != nullptr)
	{
		delete _currentWorklogList;
		_lstViewWorklogs->ClearAll();
		InitListView();
	}
	

	try
	{
		const Jira::Data::GetIssue issue = _jiraClient->get_issue(_txtSimpleJiraTicket->GetValue().ToStdString());
		std::vector<TimeTac::TimeTableEntry> timeTableEntries = TimeTac::TimeTableCsvParser::parse(_txtTimeTacCsvPath->GetValue().ToStdString());
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



		std::vector<Jira::Data::AddWorklog>* worklogs = new std::vector<Jira::Data::AddWorklog>();

		for (std::vector<std::tuple<tm, tm>>::iterator it = bookings.begin(); it != bookings.end(); ++it)
		{
			time_t begin = mktime(&std::get<0>(*it));
			time_t end = mktime(&std::get<1>(*it));

			double secondsDiff = difftime(end, begin);

			if (secondsDiff <= 0)
				continue;

			Jira::Data::AddWorklog worklog;
			worklog.set_issue_id(issue.get_id());
			worklog.set_started(std::make_shared<std::string>(to_jira_string(&std::get<0>(*it))));
			worklog.set_comment(std::make_shared<std::string>("tool-insert on " + currentDateTime));

			worklog.set_time_spent_seconds(std::make_shared<int64_t>((int64_t)(secondsDiff)));
			worklogs->push_back(worklog);
		}

		std::map<int, Jira::Data::AddWorklog>* worklogMap = new std::map<int, Jira::Data::AddWorklog>();
		int cnt = 0;
		for (std::vector<Jira::Data::AddWorklog>::reverse_iterator it = worklogs->rbegin(); it != worklogs->rend(); ++it)
		{
			wxListItem item;
			_lstViewWorklogs->InsertItem(cnt, "");
			_lstViewWorklogs->SetItem(cnt, 0, *it->get_started());

			int64_t totalSeconds = *it->get_time_spent_seconds();

			int64_t seconds = totalSeconds % 60;
			int64_t minutes = (totalSeconds / 60) % 60;
			int64_t hours = (totalSeconds / 60) / 60;

			char buffer[10];
			snprintf(buffer, 10, "%02d:%02d:%02d", hours, minutes, seconds);

			_lstViewWorklogs->SetItem(cnt, 1, std::string(buffer));
			_lstViewWorklogs->SetItem(cnt, 2, *it->get_issue_id());
			_lstViewWorklogs->SetItem(cnt, 3, "pending");

			worklogMap->insert(std::make_pair(cnt, *it));
			cnt++;
		}

		_currentWorklogList = worklogMap;

		_lstViewWorklogs->SetColumnWidth(0, wxLIST_AUTOSIZE);
		_lstViewWorklogs->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
		_lstViewWorklogs->SetColumnWidth(2, wxLIST_AUTOSIZE);
		_lstViewWorklogs->SetColumnWidth(3, wxLIST_AUTOSIZE);

		_lstViewWorklogs->Refresh();
	}
	catch (boost::wrapexcept<std::runtime_error> err)
	{
		std::cerr << err.what();
	}
	catch (boost::wrapexcept< boost::system::system_error> exc)
	{
		wxMessageBox(exc.what());
	}
	catch (std::exception e)
	{
		wxMessageBox(e.what());
	}
}

void TimeTac2Jira::MainWindow::BookWorklog(wxCommandEvent& event)
{
	if (_currentWorklogList == nullptr || _currentWorklogList->empty())
		wxMessageBox("Please load data first", "", wxOK | wxICON_ERROR, _panelBase);


	// we want to start a long task, but we don't want our GUI to block
	// while it's executed, so we use a thread to do it.
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		wxLogError("Could not create the worker thread!");
		return;
	}
	// go!
	if (GetThread()->Run() != wxTHREAD_NO_ERROR)
	{
		wxLogError("Could not run the worker thread!");
		return;
	}
}

void TimeTac2Jira::MainWindow::WorklogUpdated(TimeTac2Jira::WorklogUpdateEvent& event_)
{
	_lstViewWorklogs->SetItem(event_._id, 3, event_._status);
	_lstViewWorklogs->RefreshItem(event_._id);
}

void TimeTac2Jira::MainWindow::OnClose(wxCloseEvent&)
{
	// important: before terminating, we _must_ wait for our joinable
	// thread to end, if it's running; in fact it uses variables of this
	// instance and posts events to *this event handler
	if (GetThread() &&      // DoStartALongTask() may have not been called
		GetThread()->IsRunning())
		GetThread()->Wait();
	Destroy();
}

TimeTac2Jira::MainWindow::~MainWindow()
{
	if (_jiraClient != nullptr)
		delete _jiraClient;
	if (_currentWorklogList != nullptr)
		delete _currentWorklogList;
}


wxThread::ExitCode TimeTac2Jira::MainWindow::Entry()
{
	wxCriticalSectionLocker lock(_currentWorklogListCS);

	if (_currentWorklogList == nullptr || _currentWorklogList->empty())
		wxMessageBox("Please load data first", "", wxOK | wxICON_ERROR, _panelBase);

	for (std::map<int, Jira::Data::AddWorklog>::iterator it = _currentWorklogList->begin(); it != _currentWorklogList->end(); it++)
	{
		wxQueueEvent(GetEventHandler(), new TimeTac2Jira::WorklogUpdateEvent(it->first, "booking..."));
		_lstViewWorklogs->RefreshItem(it->first);
		bool success = _jiraClient->add_worklog_to_issue(it->second);
		if (success)
			wxQueueEvent(GetEventHandler(), new TimeTac2Jira::WorklogUpdateEvent(it->first, "OK"));
		else
			wxQueueEvent(GetEventHandler(), new TimeTac2Jira::WorklogUpdateEvent(it->first, "ERROR"));
	}
	return (wxThread::ExitCode)0;
}