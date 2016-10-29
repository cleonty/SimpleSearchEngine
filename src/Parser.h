#pragma once
#include <set>
#include <string>

class BaseToken
{
public:
	std::wstring word;
	virtual void ParseToken(const std::wstring &str, int &n) = 0;
};

class WordToken : public BaseToken
{
	public:
		void ParseToken(const std::wstring &str, int &n);
};
class DigitToken : public BaseToken
{
	public:
		void ParseToken(const std::wstring &str, int &n);
};


class TextParser
{
public:
	TextParser();
	void Parse(const std::wstring &str);
	void Reset();
public:
	std::set<std::wstring> words;

};
