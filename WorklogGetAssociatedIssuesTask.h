#pragma once

#pragma once

#include <qthreadpool.h>
#include <JiraClient.h>
#include "TimeTableEntryModel.h"

class WorklogGetAssociatedIssuesTask : public QObject, public QRunnable
{
	Q_OBJECT

public:

	WorklogGetAssociatedIssuesTask(std::string user_, std::shared_ptr<Jira::JiraHttpClient> jiraClient_, std::vector<TimeTac::TimeTableItemModel> worklogs_) : _user(user_), _jiraClient(jiraClient_), _worklogs(worklogs_)
	{

	}

signals:
	void worklog_status_changed(TimeTac::TimeTableItemModel worklog_, TimeTac::TimeTableItemModel::BookingStatus status_);
	void get_associated_issues_task_finished(TimeTac::TimeTableItemModel worklog_, Jira::Data::SearchResults* results_);
	void finished();

public:
	void run() override
	{
		Jira::Data::GetUser currentUser = _jiraClient->get_current_user();
		_userId = std::string(*currentUser.get_account_id());
		for (std::vector<TimeTac::TimeTableItemModel>::iterator it = _worklogs.begin(); it != _worklogs.end(); it++)
		{
			emit worklog_status_changed(*it, TimeTac::TimeTableItemModel::BookingStatus::GetAssociatedTickets);
			Jira::Data::SearchResults* results = get_associated_issues(*it);
			if (results == nullptr || results->get_issues() == nullptr || results->get_issues()->empty())			{
				emit worklog_status_changed(*it, TimeTac::TimeTableItemModel::BookingStatus::NoAssociatedTicketsFound);
				continue;
			}
			emit get_associated_issues_task_finished(*it, results);
			emit worklog_status_changed(*it, TimeTac::TimeTableItemModel::BookingStatus::Pending);
		}
		emit finished();
	}

private:
	std::shared_ptr<Jira::JiraHttpClient> _jiraClient;
	std::vector<TimeTac::TimeTableItemModel> _worklogs;
	std::string _user;
	std::string _userId;

	Jira::Data::SearchResults* get_associated_issues(TimeTac::TimeTableItemModel item_)
	{
		try
		{
			std::stringstream jql;
//#ifdef _DEBUG
//			tm test = tm(item_._from);
//			test.tm_year = 2020-1900;
//			test.tm_hour = 7;
//			test.tm_min = 26;
//			test.tm_mday = 17;
//			test.tm_mon = 11-1;
//			//project = FSUI AND assignee = rhintersteininger AND status changed TO "In Progress" BEFORE "-2w" AND status changed TO "Implementation Finished" AFTER "-2w"
//			/*jql << "assignee = \"" << _user << "\" AND status changed TO \"In Progress\" BEFORE \"" << TimeTac::TimeTableItemModel::to_jira_string_short(&test);
//			jql << "\" AND status changed TO \"Implementation Finished\" AFTER \"" << TimeTac::TimeTableItemModel::to_jira_string_short(&test) << "\"";*/
//			jql << "status WAS \"In Progress\" BY \""<< _userId << "\" DURING (\"" << TimeTac::TimeTableItemModel::to_jira_string_short(&test) << "\",\"" << TimeTac::TimeTableItemModel::to_jira_string_short(&test) << "\")";
//#else
			//project = FSUI AND assignee = rhintersteininger AND status changed TO "In Progress" BEFORE "-2w" AND status changed TO "Implementation Finished" AFTER "-2w"
			/*jql << "assignee = \"" << _user << "\" AND status changed TO \"In Progress\" BEFORE \"" << TimeTac::TimeTableItemModel::to_jira_string_short(&item_._from);
			jql << "\" AND status changed TO \"Implementation Finished\" AFTER \"" << TimeTac::TimeTableItemModel::to_jira_string_short(&item_._from) << "\"";*/
			jql << "status WAS \"In Progress\" BY \"" << _userId << "\" DURING (\"" << TimeTac::TimeTableItemModel::to_jira_string_short(&item_._from) << "\",\"" << TimeTac::TimeTableItemModel::to_jira_string_short(&item_._until) << "\")";
//#endif
			std::string jqlstr = jql.str();
			Jira::Data::SearchResults* results = new Jira::Data::SearchResults(_jiraClient->search(jql.str()));
			return results;
		}
		catch (boost::wrapexcept<boost::system::system_error> ex)
		{
			return nullptr;
		}
		catch (std::exception e)
		{
			return nullptr;
		}
		//Catch all jira exceptions Rework when adding http exceptions
		catch (...)
		{
			return nullptr;
		}
	}
};
