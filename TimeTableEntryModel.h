#pragma once

#include <qabstractitemmodel.h>
#include <qdatetime.h>
#include "TimeTacCsvParser.h"

namespace TimeTac
{
	class TimeTableItemModel
	{
	public:
		enum class BookingStatus
		{
			Pending,
			Booking,
			GetIssueId,
			TicketNotFound,
			Skipped,
			OK,
			Error
		};

		TimeTableItemModel()
		{

		}

		TimeTableItemModel(int id_, tm from_, tm until_, QString ticketKey_) : _id(id_), _enabled(true), _from(from_), _until(until_), _ticketKey(ticketKey_), _status(BookingStatus::Pending)
		{
			_qtFrom = QDateTime::fromTime_t(mktime(&from_));
			_qtUntil = QDateTime::fromTime_t(mktime(&until_));

		}

		int _id;

		bool _enabled;
		tm _from;
		tm _until;
		QDateTime _qtFrom;
		QDateTime _qtUntil;
		QString _ticketKey;
		BookingStatus _status;

		static QString status_to_string(BookingStatus status_);
	};

	Q_DECLARE_METATYPE(TimeTableItemModel)
	Q_DECLARE_METATYPE(TimeTac::TimeTableItemModel::BookingStatus)

	class TimeTableEntryTableModel : public QAbstractTableModel
	{
		Q_OBJECT
	public:
		TimeTableEntryTableModel(std::vector<TimeTableItemModel> initialEntries_) : _items(initialEntries_)
		{
		}

		TimeTableEntryTableModel()
		{
		}

		enum Columns
		{
			Enabled,
			From,
			Until,
			TicketKey,
			Status,
			Col_Max
		};

		// Inherited via QAbstractItemModel
		virtual Q_INVOKABLE int rowCount(const QModelIndex& parent = QModelIndex()) const override;
		virtual Q_INVOKABLE int columnCount(const QModelIndex& parent = QModelIndex()) const override;
		virtual Q_INVOKABLE QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
		virtual Q_INVOKABLE QVariant headerData(int section, Qt::Orientation oriantation, int role) const override;
		virtual Q_INVOKABLE Qt::ItemFlags flags(const QModelIndex& index) const override;
		virtual Q_INVOKABLE bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

		void add_item(TimeTableItemModel item_);
		void set_items(std::vector<TimeTableItemModel> items_);

		std::vector<TimeTableItemModel> get_items() { return _items; };
	
	private:
		std::vector<TimeTableItemModel> _items;

		QVariant get_display_role(int row_, int col_) const;
		QVariant get_checkstate_role(int row_, int col_) const;
		QVariant get_textalignment_role(int row_, int col_) const;
		QVariant get_background_role(int row_, int col_) const;
		QVariant get_foreground_role(int row_, int col_) const;

	public slots:
		void status_changed(TimeTableItemModel item_, TimeTableItemModel::BookingStatus status_);

	};
}