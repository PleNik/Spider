#include "dataBase.h"

DataBase::DataBase(ParserFileSettingsIni& parserFileSettingsIni) :
	connect("host=" + parserFileSettingsIni.getDBaseHost() +
			" port=" + parserFileSettingsIni.getDBasePort() +
			" dbname=" + parserFileSettingsIni.getDBaseName() +
			" user=" + parserFileSettingsIni.getDBaseUser() +
			" password=" + parserFileSettingsIni.getDBasePassword())
{
	CrateTables();
}

void DataBase::CrateTables()
{
	pqxx::transaction trx(connect); //создали объект транзакции

	trx.exec("CREATE TABLE IF NOT EXISTS documents ("
		" id serial,"
		" document text NOT NULL,"
		" CONSTRAINT documents_pk PRIMARY KEY(id)); ");

	trx.exec("CREATE TABLE IF NOT EXISTS words ("
		" id serial,"
		" word varchar NOT NULL, "
		" CONSTRAINT word_pk PRIMARY KEY(id)); ");

	trx.exec("CREATE TABLE IF NOT EXISTS words_usage ("
		" id serial,"
		" document_id integer NOT NULL references documents(id),"
		" word_id integer NOT NULL references words(id), "
		" CONSTRAINT word_usage_pk PRIMARY KEY(id)); ");

	trx.commit();


	
}
