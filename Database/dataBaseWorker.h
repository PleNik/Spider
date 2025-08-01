#pragma once

#include <mutex>

#include "../Database/dataBase.h"
#include "../httpClient/urlAddress.h"


class DatabaseWorker : public DataBase
{
public:
	DatabaseWorker(ParserFileSettingsIni& iniParser);

	bool documentExists(const UrlAddress& address);

	void addWords(const UrlAddress& address,
		const std::map<std::string, int>& wordsCount);

protected:
	std::mutex documentAddMutex;
};