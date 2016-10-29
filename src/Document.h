#pragma once
#include <string>
class DocCollection;

class Document
{
public:
	Document(DocCollection *pColl, std::wstring strId, const std::wstring &strTitle, const std::wstring &strDate)
		: m_pColl(pColl)
		, m_strId(strId)
		, m_strTitle(strTitle)
		, m_strDate(strDate)
	{
	}
public:
	virtual ~Document(void);
public:
	std::wstring Id() const
	{ return m_strId; }

	std::wstring Title() const
	{ return m_strTitle; }
	void SetTitle(const std::wstring &strTitle);

	std::wstring Date() const
	{ return m_strDate; }

	std::wstring Text() const;
	void SetText(const std::wstring &strText);
private:
	std::wstring m_strId;
	std::wstring m_strTitle;
	std::wstring m_strDate;

	DocCollection *m_pColl;
};
