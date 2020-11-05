#pragma once

#include <string>
#include <csv.h>
#include <ctime>

namespace TimeTac
{
	class TimeTableEntry
	{
	public:
		TimeTableEntry(tm come_, tm go_, std::vector<std::tuple<tm, tm>> pauses_, std::string comment_) : _come(come_), _go(go_), _pauses(pauses_), _comment(_comment) {}

		const tm get_come_date() { return _come; }
		const tm get_go_date() { return _go; }
		const std::vector<std::tuple<tm, tm>> get_pause_dates(){ return _pauses; }
		const std::string get_comment() { return _comment; }
	private:
		tm _come;
		tm _go;
		std::vector<std::tuple<tm, tm>> _pauses;
		std::string _comment;
	};

	class TimeTableCsvParser
	{
	public:
		static std::vector<TimeTableEntry> parse(std::string filePath_);
	private:
		static tm to_date(int day_, int month_, int year_, int hour_, int minute_)
		{
			month_--;
			year_ -= 1900;

			tm dateTime;
			dateTime.tm_year = year_;
			dateTime.tm_mon = month_;
			dateTime.tm_mday = day_;
			dateTime.tm_hour = hour_;
			dateTime.tm_min = minute_;
			
			dateTime.tm_sec = 0;

			dateTime.tm_isdst = -1;

			mktime(&dateTime);

			return dateTime;
		}
	};
}