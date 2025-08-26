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
		
		UrlAddress link = sharedAddress(startPage); //сохранили порт, хост и таргет в поля структуры UrlAddress

		int depth = parserIniFile.getRecursionDepth();
		
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

std::string getHtmlPage(const UrlAddress& address)
{
	std::string htmlPage;

	try {
		std::string host = address.hostName;
		std::string target = address.target;

		net::io_context ioc;

		if (address.protocol == ProtocolType::HTTP) {

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
			get_lowest_layer(stream).connect(resolver.resolve(host, "https"));
			get_lowest_layer(stream).expires_after(std::chrono::seconds(30));

			http::request<http::empty_body> req{ http::verb::get, target, 11 };
			req.set(http::field::host, host);
			req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

			SSL_set_tlsext_host_name(stream.native_handle(), host.c_str());

			stream.handshake(ssl::stream_base::client);
			http::write(stream, req);

			beast::flat_buffer buffer;
			http::response<http::dynamic_body> res;
			http::read(stream, buffer, res);

			htmlPage = buffers_to_string(res.body().data());

		}
	}
	catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
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
	try {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		std::string htmlPage = getHtmlPage(link);

		std::vector<std::string> rawLinks = pickOutLinks(htmlPage);

		std::unordered_set<UrlAddress> linksUnique = uniqueLinks(rawLinks, link.protocol,
			link.hostName);


		std::string text = indexer(htmlPage); //индексация веб-страницы

		//запись слов текста в вектор
		std::vector<std::string> wordsVec = splitText(text);

		//подсчет слов в тексте
		std::map<std::string, int> wordsCount = countWords(wordsVec);

		databaseWorker.addWords(link, wordsCount);

		if (depth > 0) {
			std::lock_guard<std::mutex> lock(mtx);

			size_t count = linksUnique.size();
			size_t index = 0;
			for (auto& link : linksUnique)
			{
				bool docExists = databaseWorker.documentExists(link);
				if (!docExists)
				{
					tasks.push([&databaseWorker, link, depth, index, count]() {
						linkParser(databaseWorker, link, depth - 1, index, count);
						});
					index++;
				}
			}
			cv.notify_one();
		}
	}
	catch (const std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
	
}
