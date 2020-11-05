#pragma once

#include <wx/wx.h>
#include <wx/listctrl.h>

#include "JiraClient.h"


namespace TimeTac2Jira
{
	class MainWindow : public wxFrame
	{
	public:
		MainWindow();
		~MainWindow();

	private:
		Jira::JiraHttpClient* _jiraClient;
		std::map<int, Jira::Data::AddWorklog>* _currentWorklogList;

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

	private:
		void LoadData(wxCommandEvent& event);
		void BookWorklog(wxCommandEvent& event);

		std::string to_jira_string(tm* date_)
		{
			char buffer[80];
			size_t size = strftime(buffer, 80, "%FT%T.000%z", date_);
			return std::string(buffer);
		}

		wxDECLARE_EVENT_TABLE();
	};

}