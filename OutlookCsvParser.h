#pragma once

#include <string>
#include <csv.h>
#include <ctime>

namespace Outlook
{
	class OutlookEntry
	{
	public:
		OutlookEntry(std::string title_, tm from_, tm until_) : _title(title_), _from(from_), _until(until_) {}

		const tm get_from_date() { return _from; }
		const tm get_until_date() { return _until; }
		const std::string get_title() { return _title; }

		static std::string to_string_short(const tm date_)
		{
			char buffer[80];
			size_t size = strftime(buffer, 80, "%d. %m. %Y %H:%M", &date_);
			return std::string(buffer);
		}
	private:
		std::string _title;
		tm _from;
		tm _until;
	};

	class OutlookCsvParser
	{
	public:
		static std::vector<OutlookEntry> parse(std::string filePath_);
	private:
		static tm to_date(int day_, int month_, int year_, int hour_, int minute_, int second_)
		{
			month_--;
			year_ -= 1900;

			tm dateTime;
			dateTime.tm_year = year_;
			dateTime.tm_mon = month_;
			dateTime.tm_mday = day_;
			dateTime.tm_hour = hour_;
			dateTime.tm_min = minute_;
			dateTime.tm_sec = second_;

			dateTime.tm_isdst = -1;

			mktime(&dateTime);

			return dateTime;
		}
	};
}