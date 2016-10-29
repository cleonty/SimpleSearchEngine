#pragma once
#include "DocCollection.h"

class MainFrame
{
public:
	MainFrame(LPWSTR lpszClassName);
	void Run();
	virtual ~MainFrame(void);
	static LRESULT WINAPI MainWindowProc(HWND hwnd, UINT msg,    
                                               WPARAM wparam, LPARAM lparam);
	static void OnNotify(HWND hWnd, LPARAM lParam);
	static void OnDbClick(HWND hWnd, LPARAM lparam);
	static void OnFileNew(HWND hWnd);
	static INT_PTR CALLBACK AboutProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
	static void ShowInfo(HWND hWnd);
private:
	LPWSTR m_lpszClassName;
	static const LPWSTR m_lpszTitle;
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	HWND m_hwndListView;
	RECT m_rcClient;

	DocCollection m_coll;
private:
	void InitListView();
	void InsertListViewItems();
};
