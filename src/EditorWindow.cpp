#include "StdAfx.h"
#include "EditorWindow.h"

EditorWindow::EditorWindow(HWND parent, Document *pDoc)
		: m_pDoc(pDoc)
		, lpszClassName(L"EditorWindow")
		, m_hwndParent(parent)
{
	m_hInstance = GetModuleHandle(NULL);
	WNDCLASS wc = { CS_HREDRAW | CS_VREDRAW, WindowProc, 0, 0,    
		m_hInstance, NULL,    
		NULL, NULL, NULL,    
		lpszClassName};

	RegisterClass(&wc);

	m_hWnd = CreateWindowEx(WS_EX_DLGMODALFRAME,    
		lpszClassName,    
		L"Редактор документа",   
		WS_SYSMENU| WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_DLGFRAME | WS_CLIPCHILDREN,   
		CW_USEDEFAULT, 0, CW_USEDEFAULT,    
		CW_USEDEFAULT, m_hwndParent, NULL,   
		m_hInstance, NULL);

	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	int width = rcClient.right - rcClient.left;
	int height = rcClient.bottom - rcClient.top;

	m_hwndEdit =  CreateWindowEx(WS_EX_CLIENTEDGE,
		WC_EDIT, NULL,
		ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,   
		0, 0, width,
		height, m_hWnd, (HMENU)11,   
		m_hInstance, NULL);

	Edit_SetText(m_hwndEdit, pDoc->Text().c_str());
	std::wstring strTitle = pDoc->Title() + L" - Редактор документа";
	SetWindowText(m_hWnd, strTitle.c_str());

	SetWindowLongPtr(m_hWnd, GWL_USERDATA, (LONG)this);  
	ShowWindow(m_hWnd, SW_SHOW);   
	UpdateWindow(m_hWnd);
}

EditorWindow::~EditorWindow(void)
{
	UnregisterClass(lpszClassName, m_hInstance);
}
LRESULT WINAPI EditorWindow::WindowProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	EditorWindow *This = (EditorWindow*)GetWindowLongPtr(hWnd, GWL_USERDATA);  
	switch ( msg ) { 
		 case WM_CLOSE:
			 {
				 if(Edit_GetModify(This->m_hwndEdit))
				 {
					 std::wstring strMsg = L"Сохранить изменения в документе " +
						 This->m_pDoc->Title() + L"?";
					 int answer = MessageBox(hWnd, strMsg.c_str(),
						 L"Редактор документов", MB_YESNOCANCEL | MB_ICONWARNING);
					 if(answer == IDCANCEL) return 0;
					 if(answer == IDYES) {
						 WCHAR szText[8192];
						 Edit_GetText(This->m_hwndEdit, szText, 8192);
						 This->m_pDoc->SetText(szText);
					 }
				 }
				 DestroyWindow(This->m_hWnd);
				 delete This;
			 }
			 break;
		 case WM_SIZE:
			 MoveWindow(This->m_hwndEdit, 0, 0, LOWORD(lparam), HIWORD(lparam), TRUE);
			 break;
	}
	return DefWindowProc(hWnd, msg, wparam, lparam);
}
