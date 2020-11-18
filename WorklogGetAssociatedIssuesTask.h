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
		if (currentUser.get_account_id() != nullptr)
			_userId = std::string(*currentUser.get_account_id());
		else
			_userId = _user;
		

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
			jql << "status WAS \"In Progress\" BY \"" << _userId << "\" DURING (\"" << TimeTac::TimeTableItemModel::to_jira_string_short(&item_._from) << "\",\"" << TimeTac::TimeTableItemModel::to_jira_string_short(&item_._until) << "\")";
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
