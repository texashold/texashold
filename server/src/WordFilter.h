#ifndef _WORDFILTER_H_
#define _WORDFILTER_H_

#include <set>
#include <string>

class WordFilter
{
public:
	WordFilter();
	~WordFilter();

	void Init();
	void FilterWord(std::string& word);

private:
	std::set<std::string> m_storage;
	const char** m_words;
	int m_count;
};

#endif