#pragma once 

#include <regex>

std::vector<std::string>pickOutLinks(const std::string& htmlPage) {
	
	std::vector<std::string> links;

	std::regex linkTemplate("<a href=\"(.*?)\"");

	auto words_begin = std::sregex_iterator(htmlPage.begin(), htmlPage.end(), linkTemplate);
	auto words_end = std::sregex_iterator();

	for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
		
		std::smatch match = *i;
		std::string match_str = match.str();

		//запись ссылки в вектор
		links.push_back(match_str.substr(9, match_str.size() - 10));
	}

	return links;
}