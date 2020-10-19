#pragma once
#include <wx/wx.h>
#include "MainWindow.h"

namespace TimeTac2Jira
{
	class MainApp : public wxApp
	{
	public:
		MainApp();
		~MainApp();

		virtual bool OnInit();
	private:
		MainWindow* _mainWindow = nullptr;
	};
}

