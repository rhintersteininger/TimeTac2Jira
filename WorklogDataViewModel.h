#pragma once

#include <wx/wx.h>
#include <wx/dataview.h>
#include <JiraData.h>

class WorklogDataViewItem
{
public:
	WorklogDataViewItem(int id_, Jira::Data::AddWorklog worklog_, std::string status_ = std::string("pending")) : _id(id_), _status(status_), _worklogItem(worklog_)
	{
			
	}

	int _id;
	std::string _status;
	Jira::Data::AddWorklog _worklogItem;
};


class WorklogDataViewModel : public wxDataViewVirtualListModel
{
public:
	enum
	{
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

	void SetStatus(int id_, std::string status_)
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