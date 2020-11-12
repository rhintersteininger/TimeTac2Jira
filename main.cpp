#include "TimeTac2Jira.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TimeTac2Jira w;
    w.show();
    return a.exec();
}
