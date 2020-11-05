#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>

#include "JiraClient.h"



namespace TimeTac2Jira
{
	class WorklogUpdateEvent;
	wxDEFINE_EVENT(EVT_WORKLOG_BOOKING_UPDATE, TimeTac2Jira::WorklogUpdateEvent);

	class WorklogUpdateEvent : public wxEvent
	{
	public:
		WorklogUpdateEvent(int id_, std::string status_) : wxEvent(0, EVT_WORKLOG_BOOKING_UPDATE)
		{
			_id = id_;
			_status = status_;
		}

		int _id;
		std::string _status;

		wxEvent* Clone() const { return new WorklogUpdateEvent(*this); }
	};

	typedef void (wxEvtHandler::* WorklogUpdateEventFunction)(TimeTac2Jira::WorklogUpdateEvent&);
#define WorklogUpdateEventHandler(func) wxEVENT_HANDLER_CAST(WorklogUpdateEventFunction, func)   

	// Optional: define an event table entry
#define EVT_WORKLOG_UPDATE(id, func) \
 	wx__DECLARE_EVT1(EVT_WORKLOG_BOOKING_UPDATE, id, WorklogUpdateEventHandler(func))


	class MainWindow : public wxFrame, public wxThreadHelper
	{
	public:
		MainWindow();
		~MainWindow();

	private:
		Jira::JiraHttpClient* _jiraClient;

		std::map<int, Jira::Data::AddWorklog>* _currentWorklogList;
		wxCriticalSection _currentWorklogListCS; //Protects _currentWorklogList

#pragma region UI
		wxPanel* _panelBase;

#pragma region Sizer
		wxBoxSizer* _boxMainContainer;
		wxBoxSizer* _boxSettingsContainer;
		wxFlexGridSizer* _flexGridJiraServerSettings;
		wxFlexGridSizer* _flexGridTimeTacCsvSettings;
		wxFlexGridSizer* _flexGridSimpleTicketSelector;
		wxFlexGridSizer* _flexGridConfirmDataTable;
		wxBoxSizer* _boxButtonGroup;
#pragma endregion 

#pragma region Input Settings
		wxStaticText* _lblJiraServer;
		wxTextCtrl* _txtJiraServer;

		wxStaticText* _lblJiraUsername;
		wxTextCtrl* _txtJiraUsername;

		wxStaticText* _lblJiraPassword;
		wxTextCtrl* _txtJiraPassword;

		wxStaticText* _lblTimeTacCsv;
		wxTextCtrl* _txtTimeTacCsvPath; 

		wxStaticText* _lblSimpleJiraTicket;
		wxTextCtrl* _txtSimpleJiraTicket;

		wxButton* _btnAnalyze;

#pragma endregion

#pragma region Data Visualisation
		
		wxListView* _lstViewWorklogs;
		wxButton* _btnConfirmBookings;

#pragma endregion

#pragma endregion

	protected:
		virtual wxThread::ExitCode Entry();

	private:
		void LoadData(wxCommandEvent& event);
		void BookWorklog(wxCommandEvent& event);
		void WorklogUpdated(WorklogUpdateEvent& event);

		void OnClose(wxCloseEvent&);

		void InitListView();


		std::string to_jira_string(tm* date_)
		{
			char buffer[80];
			size_t size = strftime(buffer, 80, "%FT%T.000%z", date_);
			return std::string(buffer);
		}

		wxDECLARE_EVENT_TABLE();
	};

}