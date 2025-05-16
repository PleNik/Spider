#pragma once

#include <iostream>
#include <pqxx/pqxx>

#include "../parserFileSettingsIni.h"

class DataBase {
public:
	DataBase(ParserFileSettingsIni& parserFileSettingsIni);
	void CrateTables();

private:
	pqxx::connection connect;
};