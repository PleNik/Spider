#include "dataBase.h"

DataBase::DataBase(ParserFileSettingsIni& parserIni) :
	connect("host=" + parserIni.getDBaseHost() +
			" port=" + parserIni.getDBasePort() +
			" dbname=" + parserIni.getDBaseName() +
			" user=" + parserIni.getDBaseUser() +
			" password=" + parserIni.getDBasePassword())
{
	CrateTables();
}

void DataBase::CrateTables()
{
	pqxx::transaction trx(connect);

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