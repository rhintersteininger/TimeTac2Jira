#include "OutlookCsvParser.h"
#include <boost/algorithm/string.hpp>

std::vector<Outlook::OutlookEntry> Outlook::OutlookCsvParser::parse(std::string filePath_)
{
    io::CSVReader<5, io::trim_chars<' ', '\t'>, io::double_quote_escape<',', '"'>> in(filePath_);
    in.read_header(io::ignore_extra_column, "Betreff", "Beginnt am", "Beginnt um", "Endet am", "Endet um");

    std::vector<Outlook::OutlookEntry> outlookEntries;

    std::string title;
    std::string fromDate;
    std::string fromTime;
    std::string untilDate;
    std::string untilTime;

    while (in.read_row(title, fromDate, fromTime, untilDate, untilTime)) {

        int fromMonth;
        int fromDay;
        int fromYear;
        sscanf_s(fromDate.c_str(), "%d.%d.%d", &fromDay, &fromMonth, &fromYear);

        int fromHour;
        int fromMinute;
        int fromSecond;
        sscanf_s(fromTime.c_str(), "%d:%d:%d", &fromHour, &fromMinute, &fromSecond);

        int untilMonth;
        int untilDay;
        int untilYear;
        sscanf_s(untilDate.c_str(), "%d.%d.%d", &untilDay, &untilMonth, &untilYear);

        int untilHour;
        int untilMinute;
        int untilSecond;
        sscanf_s(untilTime.c_str(), "%d:%d:%d", &untilHour, &untilMinute, &untilSecond);


        OutlookEntry ole(title, to_date(fromDay, fromMonth, fromYear, fromHour, fromMinute, fromSecond), to_date(untilDay, untilMonth, untilYear, untilHour, untilMinute, untilSecond));
        outlookEntries.push_back(ole);
    }

    return outlookEntries;
}