#include "TimeTableEntryModel.h"
#include "TicketChooserDialog.h"

#include <qbrush.h>
#include <qinputdialog.h>

void TimeTac::TimeTableItemModel::set_from_time(int hour_, int minute_)
{
	_from.tm_hour = hour_;
	_from.tm_min = minute_;

	_qtFrom = QDateTime::fromSecsSinceEpoch(mktime(&_from));
}

void TimeTac::TimeTableItemModel::set_until_time(int hour_, int minute_)
{
	_until.tm_hour = hour_;
	_until.tm_min = minute_;

	_qtUntil = QDateTime::fromSecsSinceEpoch(mktime(&_until));
}


Q_INVOKABLE int TimeTac::TimeTableEntryTableModel::rowCount(const QModelIndex& parent) const
{
	return _items.size();
}

Q_INVOKABLE int TimeTac::TimeTableEntryTableModel::columnCount(const QModelIndex& parent) const
{
	return Columns::Col_Max;
}

Q_INVOKABLE QVariant TimeTac::TimeTableEntryTableModel::data(const QModelIndex& index, int role) const
{
	int row = index.row();
	int col = index.column();

	switch (role)
	{
	case Qt::DisplayRole:
		return get_display_role(row, col);
	case Qt::CheckStateRole:
		return get_checkstate_role(row, col);
	case Qt::TextAlignmentRole:
		return get_textalignment_role(row, col);
	case Qt::BackgroundRole:
		return get_background_role(row, col);
	case Qt::ForegroundRole:
		return get_foreground_role(row, col);
	}

	return QVariant();
}

Qt::ItemFlags TimeTac::TimeTableEntryTableModel::flags(const QModelIndex& index) const
{
	int col = index.column();

	switch (col)
	{
	case Columns::Enabled:
		return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	case Columns::TicketKey:
		return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}


QString TimeTac::TimeTableItemModel::status_to_string(BookingStatus status_)
{
	switch (status_)
	{
	case BookingStatus::GetAssociatedTickets:
		return "Fetch associated Issues";
	case BookingStatus::NoAssociatedTicketsFound:
		return "No associated Issues found";
	case BookingStatus::Pending:
		return "Pending";
	case BookingStatus::Booking:
		return "Booking";
	case BookingStatus::Error:
		return "Error";
	case BookingStatus::OK:
		return "OK";
	case BookingStatus::Skipped:
		return "Skipped";
	case BookingStatus::GetIssueId:
		return "Fetch Issue Id";
	case BookingStatus::TicketNotFound:
		return "Ticket not found";
	}
	return QString();
}

QVariant TimeTac::TimeTableEntryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case Columns::Enabled:
			return QVariant();
		case Columns::From:
			return "From";
		case Columns::Status:
			return "Status";
		case Columns::TicketKey:
			return "Ticket";
		case Columns::Until:
			return "Until";
		}
	}

	return QVariant();
}

void TimeTac::TimeTableEntryTableModel::add_item(TimeTableItemModel item_, TimeTableItemModel itemBeforeNewItem_)
{
	int cnt = 0;
	std::vector<TimeTableItemModel>::iterator insertIt;
	for (std::vector<TimeTableItemModel>::iterator it = _items.begin(); it != _items.end(); ++it)
	{
		if (it->_id == itemBeforeNewItem_._id)
		{
			insertIt = it;
			break;
		}
		cnt++;
	}

	this->beginInsertRows(QModelIndex(), cnt+1, cnt+1);

	_items.insert(insertIt+1, item_);
	
	this->endInsertRows();
}

void TimeTac::TimeTableEntryTableModel::set_items(std::vector<TimeTableItemModel> items_)
{
	this->beginResetModel();
	_items = items_;
	this->endResetModel();
}

#pragma region Get Column role implementation

QVariant TimeTac::TimeTableEntryTableModel::get_display_role(int row_, int col_) const
{
	TimeTableItemModel item = _items.at(row_);

	switch (col_)
	{
	case Columns::Enabled:
		return QVariant();
	case Columns::From:
		return QDateTime::fromSecsSinceEpoch(mktime(&item._from)).toString("ddd dd.MM.yyyy hh:mm");
	case Columns::Until:
		return QDateTime::fromSecsSinceEpoch(mktime(&item._until)).toString("ddd dd.MM.yyyy hh:mm");
	case Columns::Status:
		return TimeTableItemModel::status_to_string(item._status);
	case Columns::TicketKey:
		return item._ticketKey;
	}
	return QVariant();
}

QVariant TimeTac::TimeTableEntryTableModel::get_checkstate_role(int row_, int col_) const
{
	TimeTableItemModel model = _items.at(row_);
	switch (col_)
	{
	case Columns::Enabled:
		if (model._enabled)
			return Qt::Checked;
		else
			return Qt::Unchecked;
	}
	return QVariant();
}

QVariant TimeTac::TimeTableEntryTableModel::get_textalignment_role(int row_, int col_) const
{
	if (col_ == Columns::Enabled)
		return QVariant((Qt::AlignCenter | Qt::AlignHCenter));
	return QVariant(Qt::AlignCenter);
}

QVariant TimeTac::TimeTableEntryTableModel::get_background_role(int row_, int col_) const
{
	TimeTableItemModel item = _items.at(row_);
	switch (col_)
	{
	case Columns::Status:
	{
		if (item._status == TimeTableItemModel::BookingStatus::Error || item._status == TimeTableItemModel::BookingStatus::TicketNotFound)
			return QVariant(QBrush(Qt::GlobalColor::red));
		else if (item._status == TimeTableItemModel::BookingStatus::OK)
			return QVariant(QBrush(Qt::GlobalColor::green));
		break;
	}
	}
	return QVariant();
}

QVariant TimeTac::TimeTableEntryTableModel::get_foreground_role(int row_, int col_) const
{
	return QVariant(QBrush(Qt::GlobalColor::black));
}

#pragma endregion


bool TimeTac::TimeTableEntryTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	int row = index.row();
	int col = index.column();

	TimeTableItemModel& item = _items.at(row);

	bool valueChanged = false;
	switch (col)
	{
	case Columns::Enabled:
		item._enabled = value.toBool();
		valueChanged = true;
		break;
	case Columns::TicketKey:
		item._ticketKey = value.toString();
		valueChanged = true;
		break;
	}

	if (valueChanged)
	{
		emit dataChanged(index, index);
	}
	return valueChanged;
}

void TimeTac::TimeTableEntryTableModel::status_changed(TimeTableItemModel item_, TimeTableItemModel::BookingStatus status_)
{
	int indexCnt = 0;
	for (std::vector<TimeTableItemModel>::iterator it = _items.begin(); it != _items.end(); ++it)
	{
		if (it->_id == item_._id)
		{
			it->_status = status_;
			QModelIndex idx = index(indexCnt, Columns::Status);
			emit dataChanged(idx, idx);
		}
		indexCnt++;
	}
}

TimeTac::TimeTableItemModel* TimeTac::TimeTableEntryTableModel::get_mutable_item(int id_)
{
	for (std::vector<TimeTac::TimeTableItemModel>::iterator it = _items.begin(); it != _items.end(); ++it)
	{
		if (it->_id == id_)
			return it._Ptr;
	}
	return nullptr;
}

TimeTac::TimeTableItemModel TimeTac::TimeTableEntryTableModel::split_item(int item_, int splitAtHour_, int splitAtMinute_)
{
	TimeTableItemModel* itemToSplit = get_mutable_item(item_);
	TimeTableItemModel newItem = TimeTableItemModel(*itemToSplit); //create copy of item

	int newId = 0;
	for(std::vector<TimeTableItemModel>::iterator it = _items.begin(); it != _items.end(); ++it)
	{
		if (it->_id > newId) newId = it->_id;
	}

	newItem._id = ++newId;

	itemToSplit->set_until_time(splitAtHour_, splitAtMinute_);
	newItem.set_from_time(splitAtHour_, splitAtMinute_);
	newItem._status = TimeTableItemModel::BookingStatus::Pending;

	QModelIndex idxTopLeft = index(0, 0);
	QModelIndex idxBotRight = index(_items.size()-1, Columns::Col_Max-1);
	
	add_item(newItem, *itemToSplit);

	emit dataChanged(idxTopLeft, idxBotRight);
	return newItem;
}
 

void TimeTac::TimeTableEntryTableModel::get_associated_issues_finished(TimeTableItemModel item_, Jira::Data::SearchResults* results_)
{
	std::shared_ptr<std::vector<Jira::Data::Issue>> issues = results_->get_issues();

#ifdef _DEBUG
	//issues->push_back(issues->at(0));
#endif

	if (issues->size() > 1)
	{
		QStringList tickets = QStringList();
		QStringList ticketsText = QStringList();

		std::vector<Jira::Data::GetIssue> getIssueData;

		int cnt = 1;
		for (std::vector<Jira::Data::Issue>::iterator it = issues->begin(); it != issues->end(); ++it)
		{
			Jira::Data::GetIssue issueData = _jiraClient->get_issue(*it->get_key());
			getIssueData.push_back(issueData);
		}
	

		TicketChooserDialog dialog = TicketChooserDialog(item_, getIssueData, _parent);
		
		if (dialog.exec())
		{
			QDateTime itemFrom = item_._qtFrom;
			QDateTime itemUntil = item_._qtUntil;
			int totalSeconds = itemFrom.secsTo(itemUntil);

			//issue -> <startime, totalIssueSeconds>
			std::vector< std::tuple< Jira::Data::GetIssue, int >> secondsPerIssue;

			int totalIssuesSeconds = 0;
			std::vector<Jira::Data::GetIssue> selectedIssues = dialog.get_selected_issues();
			for (std::vector<Jira::Data::GetIssue>::iterator it = selectedIssues.begin(); it != selectedIssues.end(); ++it)
			{
				int totalIssueSeconds = 0;
				std::vector<std::tuple<QDateTime, QDateTime>> timeragesForIssue = get_time_in_progess(item_, *it);
				for (std::vector<std::tuple<QDateTime, QDateTime>>::iterator timeRangeIt = timeragesForIssue.begin(); timeRangeIt != timeragesForIssue.end(); ++timeRangeIt)
				{
					QDateTime start = std::get<0>(*timeRangeIt);//qMax(std::get<0>(*timeRangeIt), item_._qtFrom);
					if (start < item_._qtFrom)
						start = item_._qtFrom;

					QDateTime end = std::get<1>(*timeRangeIt);
					if (end > item_._qtUntil)
						end = item_._qtUntil;
								
					totalIssueSeconds += start.secsTo(end);
				}
				totalIssuesSeconds += totalIssueSeconds;
				secondsPerIssue.push_back(std::make_tuple(*it, totalIssueSeconds));
			}

			TimeTableItemModel currentItem = item_;
			TimeTableItemModel nextItem;
			QDateTime itemStartTime = item_._qtFrom;

			for (std::vector<std::tuple<Jira::Data::GetIssue, int>>::iterator it = secondsPerIssue.begin(); it != secondsPerIssue.end(); ++it)
			{
				float percentage = ((100 / (float)totalIssuesSeconds) * (float)std::get<1>(*it));
				int percentageSeconds = ((totalSeconds / (float)100) * percentage);
				itemStartTime = itemStartTime.addSecs(percentageSeconds);

				//Do not split last issue
				if (it == secondsPerIssue.end()-1)
				{
					set_ticket_key(currentItem, *std::get<0>(*it).get_key());
				}
				else
				{
					nextItem = split_item((currentItem._id), itemStartTime.time().hour(), itemStartTime.time().minute());
					set_ticket_key(currentItem, *std::get<0>(*it).get_key());
					currentItem = nextItem;
				}
			}
		}
	}
	else
	{
		set_ticket_key(item_, *results_->get_issues()->begin()->get_key().get());
	}
	
}

std::vector<std::tuple<QDateTime, QDateTime>> TimeTac::TimeTableEntryTableModel::get_time_in_progess(TimeTableItemModel item_, Jira::Data::GetIssue issue_)
{
	std::vector<std::tuple<QDateTime, QDateTime>> timeranges;

	Jira::Data::Changelog changelog = *issue_.get_changelog();
	std::vector<Jira::Data::ChangeHistory> changeHistory = *changelog.get_histories();

	QDateTime* start = nullptr;
	QDateTime* end = nullptr;

	for (std::vector<Jira::Data::ChangeHistory>::reverse_iterator it = changeHistory.rbegin(); it != changeHistory.rend(); ++it)
	{
		std::string createdAt = *it->get_created();
		QDateTime createdAtTime = QDateTime::fromString(QString(createdAt.c_str()), Qt::DateFormat::ISODateWithMs);
		bool isValid = createdAtTime.isValid();
		std::vector<Jira::Data::ChangeItem> changeItem = *it->get_items();

		bool fromInProgess = false;
		bool toInProgess = false;
		for (std::vector<Jira::Data::ChangeItem>::iterator changeItemIt = changeItem.begin(); changeItemIt != changeItem.end(); ++changeItemIt)
		{
			std::string field = *changeItemIt->get_field();
			if (field.compare("status") == 0)
			{
				fromInProgess = (changeItemIt->get_from_string()->compare("In Progress") == 0);
				toInProgess = (changeItemIt->get_to_string()->compare("In Progress") == 0);
				break;
			}
		}

		if (toInProgess && start == nullptr)
		{
			start = new QDateTime(createdAtTime);
		}
		if (fromInProgess && start != nullptr)
		{
			end = new QDateTime(createdAtTime);
		}
		if (start != nullptr && end != nullptr)
		{
			timeranges.push_back(std::make_tuple(*start, *end));
			start = nullptr;
			end = nullptr;
		}
	}

	if (start != nullptr)
	{
		if (end == nullptr)
			end = &item_._qtUntil;
		timeranges.push_back(std::make_tuple(*start, *end));
	}

	std::vector<std::tuple<QDateTime, QDateTime>> finalTimeranges;

	for (std::vector<std::tuple<QDateTime, QDateTime>>::iterator it = timeranges.begin(); it != timeranges.end(); ++it)
	{
		//Started in the future
		if (std::get<0>(*it) > item_._qtUntil) continue;
		//Item ended in the past
		if (std::get<1>(*it) < item_._qtFrom) continue;

		finalTimeranges.push_back(*it);
	}

	return finalTimeranges;
}


void TimeTac::TimeTableEntryTableModel::set_ticket_key(TimeTableItemModel item_, std::string ticketKey_)
{
	TimeTableItemModel* model = get_mutable_item(item_._id);
	model->_ticketKey = QString(ticketKey_.c_str());
	emit dataChanged(index(0, Columns::TicketKey), index(_items.size() - 1, Columns::TicketKey));
}

