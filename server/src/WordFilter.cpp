#include "stdafx.h"
#include "WordFilter.h"
#include "IO/XFileStream.h"

WordFilter::WordFilter()
{
	m_words = NULL;
	Init();
}

WordFilter::~WordFilter()
{
	if(m_words) 
	{
		free(m_words);
	}
}

void WordFilter::Init()
{
	FILE* f = fopen(XFileStream::PathInConfig("sensitive_word.txt"), "rb");//0x0d 0x0a
	if(f)
	{
		fseek(f, 0, SEEK_END);
		int length = ftell(f);
		fseek(f, 0, SEEK_SET);

		char buf[128];
		int curPos = 0;//index in file
		int curIndex = 0;//index in line
		while(curPos < length)
		{
			do
			{
				fread(buf+curIndex, 1, 1, f);
				curPos++;
				curIndex++;
			}
			while(buf[curIndex - 1] != 0x0a);
			buf[curIndex - 2] = 0;
			curIndex = 0;
			if(strlen(buf) > 0)
			{
				m_storage.insert(std::string(buf));
			}
		}
		fclose(f);
	}

	m_count = m_storage.size();
	if(m_count > 0)
	{
		m_words = (const char**)malloc(sizeof(char*) * m_count);
		std::set<std::string>::iterator ptr;
		int i = 0;
		for(ptr = m_storage.begin(); ptr != m_storage.end(); ++ptr,i++) 
		{
			m_words[i] = ptr->c_str();
		}
	}
}

static inline void _filterWord(char* word, const char* lowerWord, const char* oldstr)
{
	int len = strlen(oldstr);
	const char* tmp;
	memset(word, '*', len);
	word += len;
	lowerWord += len;

	while((tmp = strstr(lowerWord, oldstr)) != NULL) 
	{
		word += (tmp-lowerWord);
		memset(word, '*', len);
		word += len;
		lowerWord = tmp + len;
	}
}

void WordFilter::FilterWord(std::string& word)
{
	std::string tmp(word);
	const char** p = (const char**)m_words;
	const char* dest;
	for(int i=0; i<m_count; i++, p++)
	{
		dest = strstr(tmp.c_str(), *p);
		if(dest) 
		{
			_filterWord((char*)(word.c_str() + (dest-tmp.c_str())), dest, *p);
		}
	}
}