#include "StdAfx.h"
#include "Resource.h"
#include "MainFrame.h"
#include "EditorWindow.h"
#include "SearchDialog.h"

const LPWSTR MainFrame::m_lpszTitle = L"Список документов";

MainFrame::MainFrame(LPWSTR lpszClassName)
	: m_lpszClassName(lpszClassName)
{
	m_hInstance = GetModuleHandle(NULL);
	WNDCLASS wc = { CS_HREDRAW | CS_VREDRAW, MainWindowProc, 0, 0,    
		m_hInstance, NULL,    
		NULL, NULL, NULL,    
		lpszClassName};

	RegisterClass(&wc);

	m_hWnd = CreateWindowEx(WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE,    
		lpszClassName,    
		m_lpszTitle,   
		WS_OVERLAPPEDWINDOW |
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS,   
		CW_USEDEFAULT, 0, CW_USEDEFAULT,    
		CW_USEDEFAULT, NULL, LoadMenu(m_hInstance, MAKEINTRESOURCE(IDC_SEARCHENGINE)),   
		m_hInstance, NULL);

	InitListView();


	SetWindowLongPtr(m_hWnd, GWL_USERDATA, (LONG)this);  
	ShowWindow(m_hWnd, SW_SHOW);   
	UpdateWindow(m_hWnd);
}

void MainFrame::InitListView()
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	int width = rcClient.right - rcClient.left;
	int height = rcClient.bottom - rcClient.top;

	m_hwndListView =  CreateWindowEx(WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR | WS_EX_STATICEDGE,
		WC_LISTVIEW, NULL,
		LVS_ALIGNTOP | LVS_EDITLABELS | LVS_REPORT | WS_CHILD | WS_HSCROLL | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,   
		0, 0, width,
		height, m_hWnd, (HMENU)11,   
		m_hInstance, NULL);

	WCHAR szText[256];  
	LVCOLUMN lvc; 
	int iCol; 
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
	for (iCol = 0; iCol < 3; iCol++) 
	{ 
		lvc.iSubItem = iCol;
		lvc.pszText = szText;    
		lvc.cx = 200;

		if ( iCol < 1 )
			lvc.fmt = LVCFMT_LEFT;
		else
			lvc.fmt = LVCFMT_RIGHT;                               

		LoadString(m_hInstance, 
			IDS_FIRSTCOLUMN + iCol, 
			szText, 
			sizeof(szText)/sizeof(szText[0]));

		ListView_InsertColumn(m_hwndListView, iCol, &lvc);
	}

	m_coll.Load();
	InsertListViewItems();
	SetFocus(m_hwndListView);
}

void MainFrame::InsertListViewItems()
{
    LVITEM lvI;
    lvI.mask = LVIF_TEXT | LVIF_STATE;  
    lvI.iSubItem = 0;
    lvI.state = 0; 
    lvI.stateMask = 0; 
    lvI.pszText = LPSTR_TEXTCALLBACK;

	for (int index = 0; index < (int)m_coll.size(); index++)
    {
        lvI.iItem = index;
		ListView_InsertItem(m_hwndListView, &lvI);
    }
}

LRESULT WINAPI MainFrame::MainWindowProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
 {
	 MainFrame *This = (MainFrame*)GetWindowLongPtr(hWnd, GWL_USERDATA);  
	 switch ( msg ) {  
		case WM_CREATE:
			{
				ShowInfo(hWnd);
				return 0;
			}
		case WM_SIZE:   
			{
				MoveWindow(This->m_hwndListView, 0, 0, LOWORD(lparam), HIWORD(lparam), TRUE);
				return 0;   
			}
		case WM_COMMAND:
			{
				switch(LOWORD(wparam)) {
		case ID_FILE_NEW:
			OnFileNew(hWnd);
			break;
		case IDM_EXIT:
			PostQuitMessage(0);
			break;
		case ID_SEARCH_INDEX:
			{
				This->m_coll.CreateIndex();
				MessageBox(hWnd, L"Выполнено.", L"Индексация документов", MB_OK);
			}
			break;
		case ID_SEARCH_FIND:
			{
				SearchDialog dlg;
				if(dlg.DoModal(This->m_hInstance, hWnd) == IDOK) {
					std::wstring strQuery(dlg.m_szQuery);
					if(!strQuery.empty()) {
						std::wstring strTitle = L"Результаты поиска - " +
							strQuery + L" - " + m_lpszTitle;
						SetWindowText(hWnd, strTitle.c_str());
						ListView_DeleteAllItems(This->m_hwndListView);
						This->m_coll.Search(strQuery);
						This->InsertListViewItems();
					}
				}
			}
			break;
		case ID_SEARCH_SHOWALL:
			{
				SetWindowText(hWnd, m_lpszTitle);
				ListView_DeleteAllItems(This->m_hwndListView);
				This->m_coll.Clear();
				This->m_coll.Load();
				This->InsertListViewItems();
			}
			break;
		case IDM_ABOUT:
			DialogBox(This->m_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutProc);  
			break;
		case IDM_INFO:
			ShowInfo(hWnd);
			break;
				}
				return 0;
			}
		case WM_CLOSE:   
			{   
				PostQuitMessage(0);   
				return 0;   
			}
		case WM_NOTIFY:
			{
				if((int)wparam == 11) {
					OnNotify(hWnd, lparam);
				}
				return 0;
			}


	 }   
	 return DefWindowProc(hWnd, msg, wparam, lparam);   
 }
void MainFrame::Run() {  
	MSG msg;   
	while ( GetMessage(&msg, NULL, 0, 0) ) {   
			TranslateMessage(&msg);   
			DispatchMessage(&msg);
		}
}  
MainFrame::~MainFrame(void)
{
	UnregisterClass(m_lpszClassName, m_hInstance);
}

void MainFrame::OnNotify(HWND hWnd, LPARAM lParam)
{
	MainFrame *This = (MainFrame*)GetWindowLongPtr(hWnd, GWL_USERDATA);  
	NMLVDISPINFO* plvdi;
	NMLISTVIEW* pnmlv;
	switch (((LPNMHDR) lParam)->code)
	{
	case LVN_GETDISPINFO:
		plvdi = (NMLVDISPINFO*) lParam;    
		switch (plvdi->item.iSubItem)
		{
		case 0:
			lstrcpyn(plvdi->item.pszText, This->m_coll[plvdi->item.iItem]->Title().c_str(),
				This->m_coll[plvdi->item.iItem]->Title().size() + 1);
			break;

		case 1:
			lstrcpyn(plvdi->item.pszText, This->m_coll[plvdi->item.iItem]->Id().c_str(),
				This->m_coll[plvdi->item.iItem]->Id().size() + 1);
			break;

		case 2:
			lstrcpyn(plvdi->item.pszText, This->m_coll[plvdi->item.iItem]->Date().c_str(),
				This->m_coll[plvdi->item.iItem]->Date().size() + 1);
			break;

		default:
			break;
		}
		break;
	case LVN_ENDLABELEDIT:
		plvdi = (NMLVDISPINFO*) lParam;
		if(plvdi->item.iSubItem == 0 && plvdi->item.pszText) {
			This->m_coll[plvdi->item.iItem]->SetTitle(plvdi->item.pszText);
		}
		break;
	case NM_DBLCLK:
		{
			pnmlv = (NMLISTVIEW*) lParam;
			if(pnmlv->iItem >= 0 && pnmlv->iSubItem == 0) {
				EditorWindow * ed = new EditorWindow(This->m_hWnd, This->m_coll[pnmlv->iItem]);
			}
		}
		break;
	}
}

void MainFrame::OnDbClick(HWND hWnd, LPARAM lparam)
{
	MainFrame *This = (MainFrame*)GetWindowLongPtr(hWnd, GWL_USERDATA);
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(lparam);
	if(pNMLV->iItem < 0) return;
	MessageBox(This->m_hWnd, This->m_coll[pNMLV->iItem]->Title().c_str(), L"", MB_OK);
}

void MainFrame::OnFileNew(HWND hWnd)
{
	MainFrame *This = (MainFrame*)GetWindowLongPtr(hWnd, GWL_USERDATA);
	Document *pDoc = This->m_coll.NewDoc();
	int iIndex = ListView_GetItemCount(This->m_hwndListView);

	LVITEM lvI;
    lvI.mask = LVIF_TEXT | LVIF_STATE;  
    lvI.iSubItem = 0;
    lvI.state = 0; 
    lvI.stateMask = 0; 
    lvI.pszText = LPSTR_TEXTCALLBACK;
    lvI.iItem = iIndex;
	ListView_InsertItem(This->m_hwndListView, &lvI);
	ListView_EnsureVisible(This->m_hwndListView, iIndex, FALSE);
	SetFocus(This->m_hwndListView);
	ListView_EditLabel(This->m_hwndListView, iIndex);
}

INT_PTR CALLBACK MainFrame::AboutProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
void MainFrame::ShowInfo(HWND hWnd)
{
	static LPCWSTR lpText = L"Программа осуществляет поиск по содержимому текстовых документов.\n"
		L"Новый документ создается вызовом команды Файл->Новый документ.\n"
		L"Редактор документов вызывается двойным щелчком по названию документа.\n"
		L"Отредактировать название документа - одинарный щелчок.\n"
		L"Документы хранятся в базе данных Microsoft Access в файле documents.mdb,\n"
		L"который должен находиться в той же папке что и программа.\n"
		L"В таблице documents хранятся идентификатор, название, дата создания и текст документов.\n"
		L"Индексация документов выполняется командой Поиск->Индексировать документы.\n"
		L"Индексируются строки из алфавитных символов и цифры.\n"
		L"Индекс хранится в плоских файлах папки index.\n"
		L"Названия файлов соответствуют словам из документов,\n"
		L"а содержимое - идентификаторы документов, где встречается данное слово.\n"
		L"Релевантностью считается количество слов запроса, которые присутствуют в документе.\n"
		L"В результаты поиска отсортированы по убыванию релевантности.\n"
		L"В результатах поиска присуствуют все документы, в которых есть хот я бы одно слово запроса.\n"
		L"Ищутся точные словоформы, как они указаны в запросе.\n"
		L"\n"
		L"Внимание!!! В программе отсутсвуют индикаторы выполнения процесса индексации и поиска!\n"
		L"\n"
		L"Недостатки решения:\n"
		L"Не учитывается морфология русского языка - словоформы `мама` и `маму` считаются разными словами.\n"
		L"Решение: Простейший выход учета морфологии делать стемминг слов при индексации `мама` -> `мам` `маму` -> `мам`.\n"
		L"Не учитываются количество повторений слов и близость слов запроса в документах при расчете релевантности.\n"
		L"Решение: В индексе следует хранить позиции слов в тексте.\n"
		L"Неэффективный способ хранения индекса.\n"
		L"Отсутствие многопоточности при индексировании.\n"
		L"Отсутствие кеширования запросов.\n"
		L"Отсутствие статистики запросов и предварительной подготовки результатов по самым высокочастотным запросам.\n"
		L"\n"
		L"Преимущества решения:\n"
		L"Простая реализация.\n"
		L"Приемлимая скорость поиска.\n";
	MessageBox(hWnd, lpText, L"Сведения о программе", MB_OK);
}

