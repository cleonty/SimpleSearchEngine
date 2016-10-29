#include "StdAfx.h"
#include "SearchDialog.h"

SearchDialog::SearchDialog(void)
{
	m_hWnd = NULL;
}

SearchDialog::~SearchDialog(void)
{
}

INT_PTR CALLBACK SearchDialog::DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SearchDialog *pDlg;

	if(uMsg == WM_INITDIALOG)
	{
		pDlg = (SearchDialog*)lParam;
		pDlg->m_hWnd = hWnd;
		SetWindowLong(hWnd, GWL_USERDATA, (LONG)pDlg);
		return (INT_PTR)TRUE;
	}

	pDlg = (SearchDialog*) GetWindowLong(hWnd, GWL_USERDATA);

	switch(uMsg) {
		case WM_COMMAND:
		{
			switch(LOWORD(wParam)) {
				case IDOK:
				{
					GetDlgItemText(hWnd, IDC_QUERY, pDlg->m_szQuery, 80);
					EndDialog(pDlg->m_hWnd, TRUE);
					return (INT_PTR)TRUE;
				}
				case IDCANCEL:
				{
					EndDialog(pDlg->m_hWnd, FALSE);
					return (INT_PTR)TRUE;
				}
			}
		}
	}
	return (INT_PTR)FALSE;
}
