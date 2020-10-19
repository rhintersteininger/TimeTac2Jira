#include "MainApp.h"

wxIMPLEMENT_APP(TimeTac2Jira::MainApp);

TimeTac2Jira::MainApp::MainApp()
{
}

TimeTac2Jira::MainApp::~MainApp()
{
}

bool TimeTac2Jira::MainApp::OnInit()
{
	_mainWindow = new MainWindow();
	_mainWindow->Show();
	return true;
}
