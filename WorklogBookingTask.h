#pragma once

#include <qthreadpool.h>
#include <JiraClient.h>
#include "TimeTableEntryModel.h"

class WorklogBookingTask : public QObject, public QRunnable
{
	Q_OBJECT

public:

	WorklogBookingTask(std::shared_ptr<Jira::JiraHttpClient> jiraClient_, std::vector<TimeTac::TimeTableItemModel> worklogs_) : _jiraClient(jiraClient_), _worklogs(worklogs_)
	{

	}

signals:
	void worklog_status_changed(TimeTac::TimeTableItemModel worklog_, TimeTac::TimeTableItemModel::BookingStatus status_);

public:
	void run() override
	{
		for (std::vector<TimeTac::TimeTableItemModel>::iterator it = _worklogs.begin(); it != _worklogs.end(); it++)
		{
			if (it->_status == TimeTac::TimeTableItemModel::BookingStatus::OK)
				continue;
			 
			if (!it->_enabled)
			{
				emit worklog_status_changed(*it, TimeTac::TimeTableItemModel::BookingStatus::Skipped);
				continue;
			}

			if (it->_ticketKey.isEmpty())
			{
				emit worklog_status_changed(*it, TimeTac::TimeTableItemModel::BookingStatus::Error);
				continue;
			}

			emit worklog_status_changed(*it, TimeTac::TimeTableItemModel::BookingStatus::Booking);
			Jira::Data::AddWorklog worklog = Jira::Data::AddWorklog();
			bool worklogSuccess = create_jira_worklog(*it, worklog);

			if (!worklogSuccess)
			{
				emit worklog_status_changed(*it, TimeTac::TimeTableItemModel::BookingStatus::TicketNotFound);
				continue;
			}

			try
			{
				bool success = _jiraClient->add_worklog_to_issue(worklog);

				if (success)
					emit worklog_status_changed(*it, TimeTac::TimeTableItemModel::BookingStatus::OK);
				else
					emit worklog_status_changed(*it, TimeTac::TimeTableItemModel::BookingStatus::Error);
			}
			//Needs Reword when adding proper exeptions to JiraClient
			catch (...)
			{
				emit worklog_status_changed(*it, TimeTac::TimeTableItemModel::BookingStatus::Error);
				continue;
			}
		}
	}



private:
	std::shared_ptr<Jira::JiraHttpClient> _jiraClient;
	std::vector<TimeTac::TimeTableItemModel> _worklogs;
	
	std::map<std::string, std::string> _issueKeyIdCache;

	bool create_jira_worklog(TimeTac::TimeTableItemModel itemModel_, Jira::Data::AddWorklog& worklog_)
	{
		qint64 secondsDiff = itemModel_._qtFrom.secsTo(itemModel_._qtUntil);
		std::string started = to_jira_string(&itemModel_._from);//(Qt::ISODateWithMs);
		std::string issueId;

		std::map<std::string, std::string>::iterator issueCacheIt = _issueKeyIdCache.find(itemModel_._ticketKey.toStdString());
		//Key Was not present in the Cache
		if (issueCacheIt == _issueKeyIdCache.end())
		{
			emit worklog_status_changed(itemModel_, TimeTac::TimeTableItemModel::BookingStatus::GetIssueId);
			try
			{
				Jira::Data::GetIssue issue = _jiraClient->get_issue(itemModel_._ticketKey.toStdString());
				issueId = *issue.get_id();
				_issueKeyIdCache.insert(std::make_pair(itemModel_._ticketKey.toStdString(), issueId));
			}
			//Needs Reword when adding proper exeptions to JiraClient
			catch (...)
			{
				return false;
			}	
		}
		else
		{
			issueId = issueCacheIt->second;
		}

		worklog_.set_started(std::make_shared<std::string>(started));
		worklog_.set_time_spent_seconds(std::make_shared<int64_t>(secondsDiff));
		worklog_.set_issue_id(std::make_shared<std::string>(issueId));
		return true;
	}

	std::string to_jira_string(tm* date_)
	{
		char buffer[80];
		size_t size = strftime(buffer, 80, "%FT%T.000%z", date_);
		return std::string(buffer);
	}
};