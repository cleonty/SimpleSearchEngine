#include "StdAfx.h"
#include <map>
#include <fstream>
#include <iterator>
#include <algorithm>
#include "DocCollection.h"
#include "Parser.h"

DocCollection::DocCollection(void)
{
	Connect();
}

DocCollection::~DocCollection(void)
{
	Clear();

	m_pRecordset->Close();
	m_pConn->Close();
}

void DocCollection::Load()
{
	try {
		m_pRecordset->Filter = L"";
		m_pRecordset->MoveFirst();
		while (!m_pRecordset->GetAdoEOF())
		{
			std::wstring strId = (WCHAR *) (_bstr_t) m_pRecordset->Fields->GetItem(L"doc_id")->Value;
			std::wstring strTitle = (WCHAR *) (_bstr_t) m_pRecordset->Fields->GetItem(L"doc_title")->Value;
			std::wstring strDate = (WCHAR *) (_bstr_t) m_pRecordset->Fields->GetItem(L"doc_date")->Value;
			Document *pDoc = new Document(this, strId, strTitle, strDate);
			push_back(pDoc);
			m_pRecordset->MoveNext();
		}
	}catch ( _com_error &e )
	{
		_bstr_t bstrSource (e.Source());
		_bstr_t bstrDescription (e.Description());

		MessageBox (NULL, (LPCTSTR) bstrDescription, (LPCTSTR) bstrSource, MB_OK);
		ExitProcess(0);
	}
}

Document *DocCollection::NewDoc()
{
	try {
		m_pRecordset->Filter = L"";
		m_pRecordset->AddNew();
		m_pRecordset->Fields->GetItem(L"doc_title")->Value = _bstr_t(L"Новый документ");
		SYSTEMTIME st;
		GetLocalTime(&st);
		WCHAR buf[64];
		wsprintf(buf, L"%02d.%02d.%d", st.wDay, st.wMonth, st.wYear);
		m_pRecordset->Fields->GetItem(L"doc_date")->Value = _bstr_t(buf);
		m_pRecordset->Fields->GetItem(L"doc_text")->Value = _bstr_t(L"");
		m_pRecordset->Update();
		std::wstring strId = (WCHAR *) (_bstr_t) m_pRecordset->Fields->GetItem(L"doc_id")->Value;
		std::wstring strTitle = (WCHAR *) (_bstr_t) m_pRecordset->Fields->GetItem(L"doc_title")->Value;
		std::wstring strDate = (WCHAR *) (_bstr_t) m_pRecordset->Fields->GetItem(L"doc_date")->Value;

		Document *pDoc = new Document(this, strId, strTitle, strDate);
		push_back(pDoc);
		return pDoc;
	}catch ( _com_error &e )
	{
		_bstr_t bstrSource (e.Source());
		_bstr_t bstrDescription (e.Description());

		MessageBox (NULL, (LPCTSTR) bstrDescription, (LPCTSTR) bstrSource, MB_OK);
		ExitProcess(0);
	}
}

void DocCollection::Connect()
{
	try{
		m_pConn.CreateInstance (__uuidof(Connection));
		m_pConn->Open (
			_bstr_t ( L"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=documents.mdb" ),
			_bstr_t ( L"" ),
			_bstr_t ( L"" ),
			adModeUnknown );

		m_pRecordset.CreateInstance (__uuidof (Recordset));
		m_pRecordset->CursorLocation = adUseClient;
		m_pRecordset->Open ( L"documents",  _variant_t((IDispatch *)m_pConn), adOpenDynamic, adLockOptimistic, adCmdTable);
		m_pRecordset->Sort = L"doc_id";
	}
	catch ( _com_error &e )
	{
		_bstr_t bstrSource (e.Source());
		_bstr_t bstrDescription (e.Description());

		MessageBox (NULL, (LPCTSTR) bstrDescription, (LPCTSTR) bstrSource, MB_OK);
		ExitProcess(0);
	}
}

void DocCollection::CleanIndexDir()
{
	WIN32_FIND_DATA fd = {0};
	HANDLE hFind = FindFirstFile(L"index\\*.*", &fd);
	if(hFind != INVALID_HANDLE_VALUE) {
		do {
			std::wstring strFilename = fd.cFileName;
			if(strFilename == L"." || strFilename == L"..") continue;
			strFilename = L"index\\" + strFilename;
			DeleteFile(strFilename.c_str());
		} while(FindNextFile(hFind, &fd));
	}
}

void DocCollection::CreateIndex()
{
	typedef std::map<std::wstring, std::vector<std::wstring> > MapWordDocs;
	MapWordDocs mapWordDocs;
	TextParser parser;

	CleanIndexDir();
	CreateDirectory(L"index", NULL);
	try {
		m_pRecordset->Filter = L"";
		m_pRecordset->MoveFirst();

		while (!m_pRecordset->GetAdoEOF())
		{
			parser.Reset();
			std::wstring strId = (WCHAR *) (_bstr_t) m_pRecordset->Fields->GetItem(L"doc_id")->Value;
			std::wstring strText = (WCHAR *) (_bstr_t) m_pRecordset->Fields->GetItem(L"doc_text")->Value;

			parser.Parse(strText);
			for(std::set<std::wstring>::const_iterator it = parser.words.begin(); 
				it != parser.words.end(); ++it) {
					mapWordDocs[*it].push_back(strId);
			}
			m_pRecordset->MoveNext();
		}
	} catch ( _com_error &e )
	{
		_bstr_t bstrSource (e.Source());
		_bstr_t bstrDescription (e.Description());

		MessageBox (NULL, (LPCTSTR) bstrDescription, (LPCTSTR) bstrSource, MB_OK);
		ExitProcess(0);
	}

	for(MapWordDocs::const_iterator it = mapWordDocs.begin();
		it != mapWordDocs.end(); ++it) {
			std::wstring strFile = L"index\\" + it->first + L".txt";
			std::wofstream out(strFile.c_str());
			std::copy(it->second.begin(), it->second.end(),
				std::ostream_iterator<std::wstring, WCHAR>(out, L" "));
	}
}

void DocCollection::Search(const std::wstring &query)
{
	TextParser parser;
	parser.Parse(query);
	std::set<std::wstring> qwords = parser.words;

	typedef std::pair<std::wstring, int> DocCountPair;
	typedef std::map<std::wstring, int> MapDocCount;
	MapDocCount mapDocCount;
	std::wstring doc;
	
	for(std::set<std::wstring>::const_iterator it = qwords.begin();
		it != qwords.end(); ++it) {
			std::wstring strFile = L"index\\" + *it + L".txt";
			std::wifstream in(strFile.c_str());
			while(in >> doc) {
				mapDocCount[doc]++;
			}
	}

	std::vector<DocCountPair> vec;
	for(MapDocCount::const_iterator it = mapDocCount.begin();
		it != mapDocCount.end(); ++it) {
			vec.push_back(DocCountPair(it->first, it->second));
	}

	struct SortPredicate {
		bool operator()(const DocCountPair &p1, const DocCountPair &p2) const
		{
			return p1.second > p2.second;
		}
	};

	std::sort(vec.begin(), vec.end(), SortPredicate());

	Clear();
	try {
		for(int i = 0; i < (int)vec.size(); ++i) {
			std::wstring strId = vec[i].first;
			std::wstring strFilter = L"doc_id=" + strId;
			m_pRecordset->Filter = _bstr_t(strFilter.c_str());
			m_pRecordset->MoveFirst();
			std::wstring strTitle = (WCHAR*)(_bstr_t)m_pRecordset->Fields->GetItem(L"doc_title")->Value;
			std::wstring strDate = (WCHAR*)(_bstr_t)m_pRecordset->Fields->GetItem(L"doc_date")->Value;
			Document *doc = new Document(this, strId, strTitle, strDate);
			push_back(doc);
		}
	}
	catch ( _com_error &e )
	{
		_bstr_t bstrSource (e.Source());
		_bstr_t bstrDescription (e.Description());

		MessageBox (NULL, (LPCTSTR) bstrDescription, (LPCTSTR) bstrSource, MB_OK);
		ExitProcess(0);
	}
}

void DocCollection::Clear()
{
	for(iterator it = begin(); it != end(); ++it)
		delete *it;
	clear();
}