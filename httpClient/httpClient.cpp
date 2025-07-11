﻿#include "httpClient.h"


int main()
{
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	std::string path_in = "D:\\Projects\\DiplomProject\\Spider\\settings.ini";

	try {
		ParserFileSettingsIni parserIniFile(path_in); //сохранили данные из settings.ini в поля класса ParserFileSettingsIni 
		
		std::string startPage = parserIniFile.getStartPage(); //получили стартовую станицу
		
		DataBase dataBase(parserIniFile); //создали в базе данных таблицы

		ParserStartPage parsPage(startPage); //сохранили порт, хост и таргет в поля класса ParserStartPage

		std::string htmlPage = getHtmlPage(parsPage); //получили html-страницу

		std::cout << htmlPage << std::endl; //удалить
	
	}
	catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}

	return 0;
}

ParserStartPage::ParserStartPage(std::string& startPage)
{
	if (startPage.find("https") != std::string::npos) {
		protocol = "https";
	}
	else {
		protocol = "http";
	}
	
	size_t posDoubleSlesh = startPage.find("//");
	std::string strAfterDoubleSlesh = startPage.substr(posDoubleSlesh + 2);

	if (strAfterDoubleSlesh.find("/") == std::string::npos) {
		host = strAfterDoubleSlesh.substr(0);
		target = "/";
	}
	else {
		size_t posSlesh = strAfterDoubleSlesh.find("/");
		host = strAfterDoubleSlesh.substr(0, posSlesh);
		target = strAfterDoubleSlesh.substr(posSlesh);
	}

}

std::string ParserStartPage::getProtocol()
{
	return protocol;
}

std::string ParserStartPage::getHost()
{
	return host;
}

std::string ParserStartPage::getTarget()
{
	return target;
}

std::string getHtmlPage(ParserStartPage& address)
{
	std::string htmlPage;

	std::string protocol = address.getProtocol();
	std::string host = address.getHost();
	std::string target = address.getTarget();

	//io_context требуется для всех операций ввода - вывода.
	net::io_context ioc;

	//Выполняет HTTP GET и выводит ответ
	if (protocol == "http") {

		//Эти объекты выполняют ввод-вывод
		tcp::resolver resolver(ioc);
		beast::tcp_stream stream(ioc);

		//Поиск доменного имени
		auto const results = resolver.resolve(host, "http");

		//Соединение по IP -адресу, который мы получаем от поиска
		stream.connect(results);

		//Настройка сообщения запроса HTTP GET
		http::request<http::string_body> req{ http::verb::get, target, 11 };
		req.set(http::field::host, host);
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

		//Отправить HTTP -запрос на удаленный хост
		http::write(stream, req);

		//Буфер для чтения
		beast::flat_buffer buffer;

		//Объявляем контейнер для ответа
		http::response<http::dynamic_body> res;

		//Получаем ответ HTTP
		http::read(stream, buffer, res);

		htmlPage = buffers_to_string(res.body().data());

		std::cout << res << std::endl;

		//Закрываем соккет
		beast::error_code ec;
		stream.socket().shutdown(tcp::socket::shutdown_both, ec);

		//Если нет соединения
		if (ec && ec != beast::errc::not_connected) {
			throw beast::system_error{ ec };
		}
	}
	else {
		//обработка https-протокола

		ssl::context ctx(ssl::context::sslv23_client);
		ctx.set_default_verify_paths();

		beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);
		stream.set_verify_mode(ssl::verify_none);

		stream.set_verify_callback([](bool preverified, ssl::verify_context& ctx) {
			return true;
		});

		ip::tcp::resolver resolver(ioc);
		get_lowest_layer(stream).connect(resolver.resolve(host, "https" ));
		get_lowest_layer(stream).expires_after(std::chrono::seconds(30));

		http::request<http::empty_body> req{ http::verb::get, target, 11 };
		req.set(http::field::host, host);
		req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

		stream.handshake(ssl::stream_base::client);
		http::write(stream, req);

		beast::flat_buffer buffer;
		http::response<http::dynamic_body> res;
		http::read(stream, buffer, res);

		htmlPage = buffers_to_string(res.body().data());

	}

	return htmlPage;
}