#pragma once

#include <iostream>
#include <map>
#include <string>

class ParserFileSettingsIni {
public:
	ParserFileSettingsIni(const std::string& fileName);

	std::string getDBaseHost();
	std::string getDBasePort();
	std::string getDBaseName();
	std::string getDBaseUser();
	std::string getDBasePassword();
	std::string getStartPage();
	int getRecursionDepth();

private:
	std::string dBaseHost;
	std::string dBasePort;
	std::string dBaseName;
	std::string dBaseUser;
	std::string dBasePassword;
	std::string startPage;
	int recursionDepth;
	int portStartSpider;
};