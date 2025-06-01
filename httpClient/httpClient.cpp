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

		ParserStartPage parsPage(startPage);
		std::string port = parsPage.getPort();
		//std::string port = "80";
		std::cout << "port: " << port << std::endl;

		std::string protocol = parsPage.getProtocol();
		std::cout << "protocol: " << protocol << std::endl;

		std::string host = parsPage.getHost();
		//std::string host = "cmake.org";
		std::cout << "host: " << host << std::endl;

		std::string target = parsPage.getTarget();
		//std::string target = "/cmake/help/latest/generator/Visual%20Studio%2017%202022.html";
		std::cout << "target: " << target << std::endl;

		int version = parsPage.getVersion();
		//int version = 11;
		std::cout << "version: " << version << std::endl;
		std::cout << std::endl;

		//Выполняет HTTP GET и выводит ответ

		//io_context требуется для всех операций ввода - вывода.
		net::io_context ioc;

		//Эти объекты выполняют наш ввод-вывод
		tcp::resolver resolver(ioc);
		beast::tcp_stream stream(ioc);

		//Поиск доменного имени
		auto const results = resolver.resolve(host, port);

		//Соединение по IP -адресу, который мы получаем от поиска
		stream.connect(results);

		//Настройка сообщения запроса HTTP GET
		http::request<http::string_body> req{ http::verb::get, target, version };
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

		std::cout << res << std::endl;

		//Закрываем соккет
		beast::error_code ec;
		stream.socket().shutdown(tcp::socket::shutdown_both, ec);

		//Если нет соединения
		if (ec && ec != beast::errc::not_connected) {
			throw beast::system_error{ ec };
		}

		//Если мы здесь, то соединение корректно закрыто
	}
	catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}

	return 0;
}

ParserStartPage::ParserStartPage(std::string& startPage)
{
	std::string _protocol = "https";
	if (startPage.substr(0, _protocol.length()) == _protocol) {
		protocol = _protocol;
	}
	else {
		protocol = "http";
	}

	size_t posDoubleSlesh = startPage.find("//");
	std::string strAfterDoubleSlesh = startPage.substr(posDoubleSlesh + 2);

	if (strAfterDoubleSlesh.find(":") != std::string::npos) {
		
		size_t posDoubleDot = strAfterDoubleSlesh.find(":");
		host = strAfterDoubleSlesh.substr(0, posDoubleDot);
		
		std::string strAfterDoubleDot = strAfterDoubleSlesh.substr(posDoubleDot + 1);
		size_t posFirstSlesh = strAfterDoubleDot.find("/");
		port = strAfterDoubleDot.substr(0, posFirstSlesh);

		target = strAfterDoubleDot.substr(posFirstSlesh + 1);
	}
	else {
		size_t posFirstSlesh = strAfterDoubleSlesh.find("/");
		host = strAfterDoubleSlesh.substr(0, posFirstSlesh);

		target = strAfterDoubleSlesh.substr(posFirstSlesh);
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

std::string ParserStartPage::getPort()
{
	return port;
}

std::string ParserStartPage::getTarget()
{
	return target;
}

int ParserStartPage::getVersion()
{
	return version;
}
