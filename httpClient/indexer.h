#pragma once

#include <iostream>
#include <string>

std::string indexer(const std::string& htmlPage)
{
	//удаление HTML-тегов
	std::regex tags("<[^>]*>");
	std::string remove;
	std::string result = std::regex_replace(htmlPage, tags, remove);

	//удаление пробелов
	std::regex r("\\s+");
	result = std::regex_replace(result, r, " ");

	//удаление знаков препинания, табуляции, переноса строк и т.д.
	std::vector<char> v_ispunct{'\t', '\n', '\r', '{', '}', '<', '>', '%', '[', ']', ':', '(', ')', ';', '?',
	'.', '~', '!', '+', '-', '*', '/', '^', '&', '|', '=', ',', '"', '#', '\\', '\v', '\f', '@', '_', '\''};

	for (size_t i = 0; i < v_ispunct.size(); i++) {
		std::string to_delete{ v_ispunct[i]};
		size_t start{ result.find(to_delete) };            // находим позицию подстроки

		while (start != std::string::npos) // находим и удаляем все вхождения to_delete
		{
			result.replace(start, 1, " ");
			start = result.find(to_delete, start + to_delete.length());
		}
	}

	result = std::regex_replace(result, r, " "); //удаление пробелов

	//перевод слов в нижний регистр
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);

	return result;
}

std::vector<std::string> splitText(std::string text) {
	std::vector<std::string> result;

	std::stringstream ss(text);

	std::string word;

	while (std::getline(ss, word, ' ')) {
		result.push_back(word);
	}

	return result;
}

std::map<std::string, int> countWords(const std::vector<std::string>& words){
	
	std::map<std::string, int> result;
	
	for (const auto& el : words) {
		
		if (el.size() >= 3 && el.size() <= 32) {
			if (result.count(el)) {
				result[el]++;
			}
			else {
				result[el] = 1;
			}
		}
	}

	return result;
}