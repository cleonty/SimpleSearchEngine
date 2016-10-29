// SearchEngine.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MainFrame.h"
#include "SearchEngine.h"
#include <locale>

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	INITCOMMONCONTROLSEX iccex = {0};
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccex.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&iccex);

	std::locale loc("");
	std::locale::global(loc);
	CoInitialize(NULL);
	MainFrame frame(L"My MainFrame");
	frame.Run();
	CoUninitialize();
	return 0;
}

