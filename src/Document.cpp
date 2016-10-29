#include "StdAfx.h"
#include "Document.h"
#include "DocCollection.h"


Document::~Document(void)
{

}

void Document::SetTitle(const std::wstring &strTitle)
{ 
	m_strTitle = strTitle;
	std::wstring strFilter = L"doc_id=" + m_strId;
	try {
		m_pColl->m_pRecordset->Filter = _bstr_t(strFilter.c_str());
		m_pColl->m_pRecordset->MoveFirst();
		m_pColl->m_pRecordset->Fields->GetItem(L"doc_title")->Value = _bstr_t(m_strTitle.c_str());
		m_pColl->m_pRecordset->Update();
	} catch ( _com_error &e )
	{
		_bstr_t bstrSource (e.Source());
		_bstr_t bstrDescription (e.Description());

		MessageBox (NULL, (LPCTSTR) bstrDescription, (LPCTSTR) bstrSource, MB_OK);
		ExitProcess(0);
	}
}

std::wstring Document::Text() const
{ 
	std::wstring strFilter = L"doc_id=" + m_strId;
	try {
		m_pColl->m_pRecordset->Filter = _bstr_t(strFilter.c_str());
		m_pColl->m_pRecordset->MoveFirst();
		return (WCHAR *) (_bstr_t) m_pColl->m_pRecordset->Fields->GetItem(L"doc_text")->Value;
	} catch ( _com_error &e )
	{
		_bstr_t bstrSource (e.Source());
		_bstr_t bstrDescription (e.Description());

		MessageBox (NULL, (LPCTSTR) bstrDescription, (LPCTSTR) bstrSource, MB_OK);
		ExitProcess(0);
	}

}
void Document::SetText(const std::wstring &strText)
{
	std::wstring strFilter = L"doc_id=" + m_strId;
	try {
		m_pColl->m_pRecordset->Filter = _bstr_t(strFilter.c_str());
		m_pColl->m_pRecordset->MoveFirst();
		m_pColl->m_pRecordset->Fields->GetItem(L"doc_text")->Value = _bstr_t(strText.c_str());
		m_pColl->m_pRecordset->Update();
	} catch ( _com_error &e )
	{
		_bstr_t bstrSource (e.Source());
		_bstr_t bstrDescription (e.Description());

		MessageBox (NULL, (LPCTSTR) bstrDescription, (LPCTSTR) bstrSource, MB_OK);
		ExitProcess(0);
	}
}