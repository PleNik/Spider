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
		" protocol int NOT NULL,"
		" host text NOT NULL, "
		" target text NOT NULL, "
		" CONSTRAINT documents_pk PRIMARY KEY(id)); ");

	trx.exec("CREATE TABLE IF NOT EXISTS words ("
		" id varchar(255),"
		" CONSTRAINT words_pk PRIMARY KEY(id)); ");

	trx.exec("CREATE TABLE IF NOT EXISTS words_usage ("
		" id serial,"
		" document_id integer NOT NULL references documents(id),"
		" word_id varchar(255) NOT NULL references words(id), "
		" quantity integer NOT NULL, "
		" CONSTRAINT words_usage_pk PRIMARY KEY(id)); ");

	trx.commit();


	
}