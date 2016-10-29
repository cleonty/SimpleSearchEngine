#pragma once
#include <vector>
#include "Document.h"

class DocCollection : public std::vector<Document*>
{
public:
	DocCollection(void);
	Document *NewDoc();
	~DocCollection(void);
	void Load();
	void Search(const std::wstring &query);
	_RecordsetPtr m_pRecordset;
	void CreateIndex();
	void Clear();
protected:
	void Connect();
	static void CleanIndexDir();
private:
	_ConnectionPtr m_pConn;
};
