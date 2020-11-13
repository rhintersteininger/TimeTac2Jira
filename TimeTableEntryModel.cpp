#include "TimeTableEntryModel.h"

#include <qbrush.h>


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
		return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
	case Columns::TicketKey:
		return Qt::ItemIsEditable | Qt::ItemIsEnabled;
	}
	return Qt::NoItemFlags;
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
	_items.push_back(item_);
	this->insertRow(_items.size() - 1);
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