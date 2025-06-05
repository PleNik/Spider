#include <fstream>

#include "parserFileSettingsIni.h"

ParserFileSettingsIni::ParserFileSettingsIni(const std::string& fileName) {
	std::ifstream settingsIni(fileName);

	if (!settingsIni.is_open()) {
		throw std::runtime_error("File " + fileName + " not found");
	}

	std::string str;
	std::map<std::string, std::string> mapSettings;

	while (std::getline(settingsIni, str)) {
		size_t posEqual = str.find('=');

		std::string strBeforeEqual = str.substr(0, posEqual);
		std::string strAfterEqual = str.substr(posEqual + 1);

		mapSettings[strBeforeEqual] = strAfterEqual;
	}
	
	dBaseHost = mapSettings["dBaseHost"];
	dBasePort = mapSettings["dBasePort"];
	dBaseName = mapSettings["dBaseName"];
	dBaseUser = mapSettings["dBaseUser"];
	dBasePassword = mapSettings["dBasePassword"];
	startPage = mapSettings["startPage"];
	recursionDepth = std::stoi(mapSettings["recursionDepth"]);
	portStartSpider = std::stoi(mapSettings["portStartSpider"]);

	settingsIni.close();
}

std::string ParserFileSettingsIni::getDBaseHost()
{
	return dBaseHost;
}

std::string ParserFileSettingsIni::getDBasePort()
{
	return dBasePort;
}

std::string ParserFileSettingsIni::getDBaseName()
{
	return dBaseName;
}

std::string ParserFileSettingsIni::getDBaseUser()
{
	return dBaseUser;
}

std::string ParserFileSettingsIni::getDBasePassword()
{
	return dBasePassword;
}

std::string ParserFileSettingsIni::getStartPage()
{
	return startPage;
}

int ParserFileSettingsIni::getRecursionDepth()
{
	return recursionDepth;
}