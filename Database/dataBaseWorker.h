#pragma once

#include <mutex>

#include "../Database/dataBase.h"
#include "../httpClient/urlAddress.h"


class DatabaseWorker : public DataBase
{
public:
	DatabaseWorker(ParserFileSettingsIni& iniParser);

	bool documentExists(const UrlAddress& address);

	void addDocumentWithWordsIfNotExists(const UrlAddress& address,
		const std::map<std::string, int>& wordCount);

protected:
	std::mutex documentAddMutex;
};