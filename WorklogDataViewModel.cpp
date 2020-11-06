#include "WorklogDataViewModel.h"

void WorklogDataViewModel::GetValueByRow(wxVariant& variant, unsigned row, unsigned col) const {
	const WorklogDataViewItem worklog = _worklogs->at(row);
	GetVariantFromCol(variant, worklog, col);
}

bool WorklogDataViewModel::SetValueByRow(const wxVariant& variant, unsigned row, unsigned col)
{
	try
	{
		WorklogDataViewItem* item = &_worklogs->at(row);
		switch (col)
		{
		case Col_Started:
			item->_worklogItem.set_started(std::make_shared<std::string>(variant.GetString()));
			return true;
		case Col_TimeSpentSeconds:
		{
			wxString timeSpent = variant.GetString();
			if (timeSpent.Contains(":"))
			{
				int hours = 0; 
				int minutes = 0;
				int seconds = 0;

				sscanf_s(timeSpent.c_str(), "%d:%d:%d", &hours, &minutes, &seconds);
				long totalSeconds = (hours * 60 * 60) + (minutes * 60) + seconds;
				item->_worklogItem.set_time_spent_seconds(std::make_shared<int64_t>(totalSeconds));

				return true;
			}
			else
			{
				item->_worklogItem.set_time_spent_seconds(std::make_shared<int64_t>(std::atol(variant.GetString())));
				return true;
			}
		}
		case Col_IssueId:
			item->_worklogItem.set_issue_id(std::make_shared<std::string>(variant.GetString()));
			return true;
		case Col_Status:
			item->_status = variant.GetString();
			return true;
		}
	}
	catch (std::exception e)
	{
		wxMessageBox(e.what());
	}
	return false;
}

void WorklogDataViewModel::GetVariantFromCol(wxVariant& variant_, WorklogDataViewItem worklog_, unsigned col) const
{
	switch (col)
	{
	case Col_Started:
	{
		std::shared_ptr<std::string> startTime = worklog_._worklogItem.get_started();
		if (startTime != nullptr)
			variant_ = *startTime;
		else
			variant_ = std::string("");
		break;
	}
	case Col_TimeSpentSeconds:
	{
		std::shared_ptr<int64_t> timeSpentSeconds = worklog_._worklogItem.get_time_spent_seconds();
		std::string formatedString = std::string("00:00:00");
		if (timeSpentSeconds != nullptr)
		{
			int totalSeconds = *timeSpentSeconds;

			int8_t seconds = totalSeconds % 60;
			int8_t minutes = (totalSeconds / 60) % 60;
			int8_t hours = (totalSeconds / 60) / 60;

			char buffer[10];
			snprintf(buffer, 10, "%02d:%02d:%02d", hours, minutes, seconds);
			formatedString = std::string(buffer);
		}
		variant_ = formatedString;

		break;
	}
	case Col_IssueId:
	{
		std::shared_ptr<std::string> issueId = worklog_._worklogItem.get_issue_id();
		if (issueId != nullptr)
			variant_ = *issueId;
		else
			variant_ = "";

		break;
	}
	case Col_Status:
		variant_ = worklog_._status;
		break;
	default:
		throw std::exception("Column not supported:" + col);
	}
}

