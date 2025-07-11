﻿#pragma once

#include <iostream>
#include <string>

//подключаем библиотеку Boost
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <openssl/ssl.h>

#include "../parserFileSettingsIni.h"
#include "../Database/dataBase.h"
//#include <Windows.h>

#pragma execution_character_set("utf-8")

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ip = boost::asio::ip;
namespace ssl = boost::asio::ssl;
using boost::asio::ip::tcp;
using std::placeholders::_1;
using std::placeholders::_2;

using tcp = net::ip::tcp;

enum class ProtocolType
{
	HTTP = 0,
	HTTPS = 1
};

class ParserStartPage {
public:
	ParserStartPage(std::string& startPage);

	std::string getProtocol();
	std::string getHost();
	std::string getTarget();

private:
	std::string protocol;
	std::string host;
	std::string target;
};

std::string getHtmlPage(ParserStartPage& address);