#include "TimeTableEntryModel.h"

#include <qbrush.h>

void TimeTac::TimeTableItemModel::set_from_time(int hour_, int minute_)
{
	_from.tm_hour = hour_;
	_from.tm_min = minute_;

	_qtFrom = QDateTime::fromTime_t(mktime(&_from));
}

void TimeTac::TimeTableItemModel::set_until_time(int hour_, int minute_)
{
	_until.tm_hour = hour_;
	_until.tm_min = minute_;

	_qtUntil = QDateTime::fromTime_t(mktime(&_until));
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

void TimeTac::TimeTableEntryTableModel::add_item(TimeTableItemModel item_)
{
	int cnt = 0;
	std::vector<TimeTableItemModel>::iterator insertIt;
	for (std::vector<TimeTableItemModel>::iterator it = _items.begin(); it != _items.end(); ++it)
	{
		if (it->_id == item_._id)
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
		return QDateTime::fromTime_t(mktime(&item._from)).toString("ddd dd.MM.yyyy hh:mm");
	case Columns::Until:
		return QDateTime::fromTime_t(mktime(&item._until)).toString("ddd dd.MM.yyyy hh:mm");
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

void TimeTac::TimeTableEntryTableModel::split_item(int item_, int splitAtHour_, int splitAtMinute_)
{
	TimeTableItemModel* itemToSplit = get_mutable_item(item_);
	TimeTableItemModel newItem = TimeTableItemModel(*itemToSplit); //create copy of item

	itemToSplit->set_until_time(splitAtHour_, splitAtMinute_);
	newItem.set_from_time(splitAtHour_, splitAtMinute_);

	QModelIndex idxTopLeft = index(0, 0);
	QModelIndex idxBotRight = index(_items.size()-1, Columns::Col_Max-1);
	
	add_item(newItem);

	emit dataChanged(idxTopLeft, idxBotRight);
}
