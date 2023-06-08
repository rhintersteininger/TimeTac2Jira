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

    const QString QACTION_SPLIT_TIMERANGE = "Split";

public slots:
    void load_timetac_csv_file(bool checked_ = false);
    void custom_context_menu_requested(QPoint point_);
    void load_csv_data();
    void book_worklog();
    void time_table_context_menu_triggered(QAction* action_);
    void fetch_associated_issues_finished();
    void use_auto_ticket_search_changed(int state_);
    void load_outlook_csv_file(bool checked_ = false);

private:
    Ui::TimeTac2JiraClass ui;

    QString _loadedTimeTacCsvFileName;
    QString _loadedOutlookCsvFileName;
    std::shared_ptr<Jira::JiraHttpClient> _jiraClient;
    TimeTac::TimeTableEntryTableModel _timeTableEntryTableModel = TimeTac::TimeTableEntryTableModel(this);

    void test_jira_connection(bool checked);
    void setup_ui();
    void bind_signal_slots();
    void prefill_data_with_events();

//private slots:
//    void worklog_status_changed(TimeTac::TimeTableItemModel worklog_, TimeTac::TimeTableItemModel::BookingStatus status_);

};
