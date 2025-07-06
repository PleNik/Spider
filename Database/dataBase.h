#pragma once

#include <iostream>
#include <pqxx/pqxx>

#include "../parserFileSettingsIni.h"

class DataBase {
public:
	DataBase(ParserFileSettingsIni& parserIni);
	void CrateTables();

protected:
	pqxx::connection connect;
};