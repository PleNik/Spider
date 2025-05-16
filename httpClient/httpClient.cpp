#include "httpClient.h"


int main()
{
	//setlocale(LC_ALL, "ru");
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	std::string path_in = "D:\\Projects\\DiplomProject\\Spider\\settings.ini";

	try {
		ParserFileSettingsIni parserIniFile(path_in); //сохранили данные из settings.ini в поля класса ParserFileSettingsIni 
		std::string startPage = parserIniFile.getStartPage(); //получили стартовую станицу
		DataBase dataBase(parserIniFile); //создали в базе данных таблицы



	}
	catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}

	return 0;
}

