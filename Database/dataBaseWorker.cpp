#include "dataBaseWorker.h"

DatabaseWorker::DatabaseWorker(ParserFileSettingsIni& iniParser) :
	DataBase(iniParser)
{
	connect.prepare("insert_document", "INSERT INTO documents (protocol, host, query) VALUES ($1, $2, $3) RETURNING id;");
	connect.prepare("insert_word", "INSERT INTO words (id) VALUES ($1) ON CONFLICT (id) DO NOTHING;");
	connect.prepare("insert_word_for_document", "INSERT INTO words_documents (word_id, document_id, amount) VALUES ($1, $2, $3) RETURNING id;");
	connect.prepare("document_count", "SELECT COUNT(*) from documents where protocol=$1 and host=$2 and query=$3");
}

bool DatabaseWorker::documentExists(const UrlAddress& address)
{
	pqxx::work txn{ connect };

	pqxx::result r = txn.exec_prepared("document_count", static_cast<int>(address.protocol), address.hostName, address.target);

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

void DatabaseWorker::addDocumentWithWordsIfNotExists(const UrlAddress& address, const std::map<std::string, int>& wordCount)
{
	std::lock_guard<std::mutex> lg(documentAddMutex);

	bool docExists = documentExists(address);
	if (docExists)
	{
		return;
	}

	pqxx::work doc_trx{ connect };

	pqxx::result r = doc_trx.exec_prepared("insert_document", static_cast<int>(address.protocol), address.hostName, address.target);

	doc_trx.commit();

	if (!r.empty()) {
		int id = r[0][0].as<int>();

		pqxx::work words_trx{ connect };

		for (auto w : wordCount)
		{
			words_trx.exec_prepared("insert_word", w.first);
			words_trx.exec_prepared("insert_word_for_document", w.first, id, w.second);
		}

		words_trx.commit();
	}
}
