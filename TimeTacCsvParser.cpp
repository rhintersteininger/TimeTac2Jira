#include "TimeTacCsvParser.h"
#include <boost/algorithm/string.hpp>

std::vector<TimeTac::TimeTableEntry> TimeTac::TimeTableCsvParser::parse(std::string filePath_)
{
    io::CSVReader<5, io::trim_chars<' ', '\t'>, io::double_quote_escape<';', '"'>> in(filePath_);
    in.read_header(io::ignore_extra_column, "Datum", "K", "G", "Pausen", "Kommentar");

    std::vector<TimeTac::TimeTableEntry> timeTableEntries;

    std::string date;
    std::string come;
    std::string go;
    std::string pauses;
    std::string comment;
    while (in.read_row(date, come, go, pauses, comment)) {
        if (date.empty()) continue;
        int month;
        int day;
        int year;
        sscanf_s(date.c_str(), "%d-%d-%d", &year, &month, &day);

        int comeHour;
        int comeMinute;
        sscanf_s(come.c_str(), "%d:%d", &comeHour, &comeMinute);

        int goHour;
        int goMinute;
        sscanf_s(go.c_str(), "%d:%d", &goHour, &goMinute);

        std::vector<std::tuple<tm, tm>> pauseVector;

        if (!pauses.empty())
        {
            std::vector<std::string> splittedPauses;
            boost::split(splittedPauses, pauses, [](char c) {return c == ';'; });

            for (std::vector<std::string>::iterator it = splittedPauses.begin(); it != splittedPauses.end(); ++it) {
                int pauseHourFrom;
                int pauseMinuteFrom;
                int pauseHourUntil;
                int pauseMinuteUntil;

                sscanf_s(it->c_str(), "%d:%d-%d:%d", &pauseHourFrom, &pauseMinuteFrom, &pauseHourUntil, &pauseMinuteUntil);

                pauseVector.push_back(std::make_tuple<tm, tm>(to_date(day, month, year, pauseHourFrom, pauseMinuteFrom), to_date(day, month, year, pauseHourUntil, pauseMinuteUntil)));
            }
        }

        TimeTableEntry tte(to_date(day, month, year, comeHour, comeMinute), to_date(day, month, year, goHour, goMinute), pauseVector, comment);
        timeTableEntries.push_back(tte);
    }

    return timeTableEntries;
}