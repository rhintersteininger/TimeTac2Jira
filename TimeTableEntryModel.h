#pragma once

#include <qabstractitemmodel.h>
#include <qdatetime.h>
#include "TimeTacCsvParser.h"
#include <QtConcurrent/qtconcurrentrun.h>
#include <JiraData.h>

namespace TimeTac
{
	class TimeTableItemModel
	{

	public:
		enum class BookingStatus
		{
			GetAssociatedTickets,
			NoAssociatedTicketsFound,
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
		void set_from_time(int hour_, int minutes_);
		void set_until_time(int hour_, int minutes_);

		static std::string to_jira_string(tm* date_)
		{
			char buffer[80];
			size_t size = strftime(buffer, 80, "%FT%T.000%z", date_);
			return std::string(buffer);
		}
		
		static std::string to_jira_string_short(tm* date_)
		{
			char buffer[80];
			size_t size = strftime(buffer, 80, "%Y/%m/%d %H:%M", date_);
			return std::string(buffer);
		}

		static std::string to_time(tm* date_)
		{
			char buffer[80];
			size_t size = strftime(buffer, 80, "%H:%M", date_);
			return std::string(buffer);
		}
	};

	Q_DECLARE_METATYPE(TimeTac::TimeTableItemModel)
	Q_DECLARE_METATYPE(TimeTac::TimeTableItemModel::BookingStatus)

	class TimeTableEntryTableModel : public QAbstractTableModel
	{
		Q_OBJECT
	public:
		TimeTableEntryTableModel(QWidget* parent_, std::vector<TimeTableItemModel> initialEntries_) : _parent(parent_), _items(initialEntries_)
		{
		}

		TimeTableEntryTableModel(QWidget* parent_) : _parent(parent_)
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

		TimeTableItemModel get_item_at(int row_) { return _items.at(row_); }
		std::vector<TimeTableItemModel> get_items() { return _items; };

		void split_item(int item_, int splitAtHour_, int splitAtMinute_);

	
	private:
		std::vector<TimeTableItemModel> _items;
		QWidget* _parent;

		QVariant get_display_role(int row_, int col_) const;
		QVariant get_checkstate_role(int row_, int col_) const;
		QVariant get_textalignment_role(int row_, int col_) const;
		QVariant get_background_role(int row_, int col_) const;
		QVariant get_foreground_role(int row_, int col_) const;

		TimeTableItemModel* get_mutable_item(int id_);

		void set_ticket_key(TimeTableItemModel item_, std::string ticketKey_);

	public slots:
		void status_changed(TimeTableItemModel item_, TimeTableItemModel::BookingStatus status_);
		void get_associated_issues_finished(TimeTableItemModel item_, Jira::Data::SearchResults* results_);

	};
}