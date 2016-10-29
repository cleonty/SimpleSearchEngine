#pragma once
#include "Document.h"

class EditorWindow
{
public:
	EditorWindow(HWND parent, Document *pDoc);
	static LRESULT WINAPI WindowProc(HWND hWnd, UINT msg,    
                                               WPARAM wparam, LPARAM lparam);
public:
	virtual ~EditorWindow(void);
private:
	Document *m_pDoc;
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	HWND m_hwndEdit;
	HWND m_hwndParent;
	LPWSTR lpszClassName;
};
