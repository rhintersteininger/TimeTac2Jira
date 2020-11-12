#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TimeTac2Jira.h"
#include <JiraClient.h>
#include "TimeTableEntryModel.h"


class TimeTac2Jira : public QMainWindow
{
    Q_OBJECT

public:
    TimeTac2Jira(QWidget *parent = Q_NULLPTR);

public slots:
    void load_timetac_csv_file(bool checked_ = false);

private:
    Ui::TimeTac2JiraClass ui;

    QString _loadedFileName;
    std::shared_ptr<Jira::JiraHttpClient> _jiraClient;
    TimeTac::TimeTableEntryTableModel _timeTableEntryTableModel;

    void setup_ui();
    void bind_signal_slots();
    void load_csv_data();
    void book_worklog();

//private slots:
//    void worklog_status_changed(TimeTac::TimeTableItemModel worklog_, TimeTac::TimeTableItemModel::BookingStatus status_);

};
