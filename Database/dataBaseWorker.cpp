#include "dataBaseWorker.h"

DatabaseWorker::DatabaseWorker(ParserFileSettingsIni& iniParser) :
	DataBase(iniParser)
{
	connect.prepare("insert_into_documents", "INSERT INTO documents (protocol, host, target) VALUES ($1, $2, $3) RETURNING id;");
	connect.prepare("insert_into_words", "INSERT INTO words (id) VALUES ($1) ON CONFLICT (id) DO NOTHING;");
	connect.prepare("insert_into_words_usage", "INSERT INTO words_usage (word_id, document_id, quantity) VALUES ($1, $2, $3) RETURNING id;");
	connect.prepare("count_of_documents", "SELECT COUNT(*) from documents where protocol=$1 and host=$2 and target=$3");
}

bool DatabaseWorker::documentExists(const UrlAddress& address)
{
	pqxx::work txn{ connect };

	pqxx::result r = txn.exec_prepared("count_of_documents", static_cast<int>(address.protocol), address.hostName, address.target);

	txn.commit();

	if (!r.empty()) {
		int id = r[0][0].as<int>();

		if (id == 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}

void DatabaseWorker::addWords(const UrlAddress& address, const std::map<std::string, int>& wordsCount)
{
	std::lock_guard<std::mutex> lg(documentAddMutex);

	bool docExists = documentExists(address);
	if (docExists)
	{
		return;
	}

	pqxx::work doc_trx{ connect };

	pqxx::result r = doc_trx.exec_prepared("insert_into_documents", static_cast<int>(address.protocol), address.hostName, address.target);

	doc_trx.commit();

	if (!r.empty()) {
		int id = r[0][0].as<int>();

		pqxx::work words_trx{ connect };

		for (auto w : wordsCount)
		{
			words_trx.exec_prepared("insert_into_words", w.first);
			words_trx.exec_prepared("insert_into_words_usage", w.first, id, w.second);
		}

		words_trx.commit();
	}
}
