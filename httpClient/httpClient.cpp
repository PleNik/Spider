#include "httpClient.h"

std::mutex mtx;
bool stoppedPoolOfThreads = false;
std::condition_variable cv;
std::queue<std::function<void()>> tasks;

int main()
{
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	std::string path_in = "D:\\Projects\\DiplomProject\\Spider\\settings.ini";

	try {
		ParserFileSettingsIni parserIniFile(path_in); //сохранили данные из settings.ini в поля класса ParserFileSettingsIni 
		
		DatabaseWorker databaseWorker(parserIniFile);

		std::string startPage = parserIniFile.getStartPage(); //получили стартовую станицу
		
		DataBase dataBase(parserIniFile); //создали в базе данных таблицы

		UrlAddress link = sharedAddress(startPage); //сохранили порт, хост и таргет в поля структуры UrlAddress

		int depth = parserIniFile.getRecursionDepth();

		std::cout << "HostName: " << link.hostName << "\tProtocol: " //удалить
			<< static_cast<int>(link.protocol) << "\tTarget: "
			<< link.target << std::endl;

		int numberOfThreads = std::thread::hardware_concurrency();

		std::vector<std::thread> poolOfThreads;

		for (size_t i = 0; i < numberOfThreads; i++) {
			poolOfThreads.emplace_back(poolOfThreadsWorker);
		}

		{
			std::lock_guard<std::mutex> lg(mtx);
			tasks.push([&databaseWorker, link, depth]() { linkParser(databaseWorker, link, depth, 1, 1); });
			cv.notify_one();
		}

		std::this_thread::sleep_for(std::chrono::seconds(2));

		{
			std::lock_guard<std::mutex> lg(mtx);
			stoppedPoolOfThreads = true;
			cv.notify_all();
		}


		for (auto& thread : poolOfThreads) {
			thread.join();
		}
	
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

std::string getHtmlPage(const UrlAddress& address)
{
	std::string htmlPage;

	std::string host = address.hostName;
	std::string target = address.target;

	//io_context требуется для всех операций ввода - вывода.
	net::io_context ioc;

	//Выполняет HTTP GET и выводит ответ
	if (address.protocol == ProtocolType::HTTP) {

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

void  poolOfThreadsWorker() {
	std::unique_lock<std::mutex> ul(mtx);

	while (!stoppedPoolOfThreads || !tasks.empty()) {
		if (tasks.empty()) {
			cv.wait(ul);
		}
		else {
			std::function<void()> task = tasks.front();
			tasks.pop();
			ul.unlock();
			task();
			ul.lock();
		}
	}
}

void linkParser(DatabaseWorker& databaseWorker, UrlAddress link, int depth, int index, int ofTotalCount)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	std::string htmlPage = getHtmlPage(link);

	if (htmlPage.size() == 0)
	{
		std::cout << "Analysis error: " << link.hostName << " " << link.target << std::endl;

		return;
	}

	std::vector<std::string> rawLinks = pickOutLinks(htmlPage);

	std::unordered_set<UrlAddress> linksUnique = uniqueLinks(rawLinks, link.protocol,
		link.hostName);

	std::string text = removeHtmlTags(htmlPage); //удаление html-тегов

	std::cout << text << std::endl; //удалить
}
