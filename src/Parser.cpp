#include "StdAfx.h"
#include "Parser.h"

void WordToken::ParseToken(const std::wstring &str, int &n)
{
	while(_istalpha(str[n]) && n < (int)str.size()) {
		word += _totlower(str[n++]);
	}
}

void DigitToken::ParseToken(const std::wstring &str, int &n)
{
	while(_istdigit(str[n]) && n < (int)str.size()) {
		word += str[n++];
	}
}

void TextParser::Parse(const std::wstring &str)
{
	int len = (int)str.size();
	for(int i = 0; i < len;)
	{
		if(!_istalnum(str[i])) {
			++i;
			continue;
		}
		BaseToken *token;
		if(_istalpha(str[i])) {
			token = new WordToken;
		} else if(_istdigit(str[i])) {
			token = new DigitToken;
		}
		token->ParseToken(str, i);
		if(!token->word.empty())
			words.insert(token->word);
		delete token;
	}
}
TextParser::TextParser()
{

}
void TextParser::Reset()
{
	words.clear();
}
