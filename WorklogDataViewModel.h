#pragma once

#include <wx/wx.h>
#include <wx/dataview.h>
#include <map>
#include <JiraData.h>

class WorklogDataViewItem
{
public:
	const static enum class BookingStatus
	{
		Pending,
		Skipped,
		Booking,
		Ok,
		Error
	};

	WorklogDataViewItem(int id_, Jira::Data::AddWorklog worklog_, BookingStatus status_ = BookingStatus::Pending) : _id(id_), _status(status_), _worklogItem(worklog_), _enabled(true)
	{
			
	}

	int _id;
	BookingStatus _status;
	bool _enabled;
	Jira::Data::AddWorklog _worklogItem;

	static std::string GetStatusString(BookingStatus status_)
	{
		return _statusMapping.at(status_);
	}
private:
	static const std::map<BookingStatus, std::string> _statusMapping;
};


class WorklogDataViewModel : public wxDataViewVirtualListModel
{
public:
	enum
	{
		Col_Enabled,
		Col_Started,
		Col_TimeSpentSeconds,
		Col_IssueKey,
		Col_IssueId,
		Col_Status,
		Col_Max
	};

	

	WorklogDataViewModel(std::vector<WorklogDataViewItem>* worklog_ = nullptr) : wxDataViewVirtualListModel(worklog_ == nullptr ? 0 : worklog_->size())
	{
		if (worklog_ == nullptr)
			worklog_ = new std::vector<WorklogDataViewItem>();
		_worklogs = worklog_;
	}

	void AppendItem(WorklogDataViewItem worklogItem_)
	{
		_worklogs->push_back(worklogItem_);
		RowAppended();
	}

	void SetStatus(int id_, WorklogDataViewItem::BookingStatus status_)
	{
		int cnt = 0;
		for (std::vector<WorklogDataViewItem>::iterator it = _worklogs->begin(); it != _worklogs->end(); ++it)
		{
			if (it->_id == id_)
			{
				it->_status = status_;
				RowValueChanged(cnt, Col_Status);
				break;
			}
			cnt++;
		}
	}

	std::vector<WorklogDataViewItem>* GetData() { return _worklogs; }
	

		
protected:
	virtual unsigned int GetColumnCount() const wxOVERRIDE { return Col_Max; }
	virtual wxString GetColumnType(unsigned int col) const wxOVERRIDE
	{
		switch (col)
		{
		case Col_Enabled: return wxT("bool");
		case Col_Started: return wxT("string"); 
		case Col_TimeSpentSeconds: return wxT("long");    
		case Col_IssueId: return wxT("string"); 
		case Col_Status: return wxT("string"); 
		}
		return wxT("string");
	}
	virtual void GetValueByRow(wxVariant& variant, unsigned row, unsigned col) const wxOVERRIDE;
	virtual bool SetValueByRow(const wxVariant& variant, unsigned row, unsigned col) wxOVERRIDE;
	virtual unsigned int GetCount() const wxOVERRIDE { return _worklogs->size(); }

private:
	std::vector<WorklogDataViewItem>* _worklogs;
	void GetVariantFromCol(wxVariant& variant_, WorklogDataViewItem worklog_, unsigned col) const;

};