#pragma once
#include "resource.h"

class SearchDialog
{
	HWND m_hWnd;
public:
	enum {marker = IDD_SEARCH_DIALOG};
	SearchDialog(void);
	
	WCHAR m_szQuery[80];
	static INT_PTR CALLBACK DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	int DoModal(HINSTANCE hInstance, HWND hWndParent = NULL)
	{
		return (int)::DialogBoxParam(hInstance, MAKEINTRESOURCE(marker), hWndParent, DialogProc, (LPARAM)(this));
	}

public:
	virtual ~SearchDialog(void);
};
