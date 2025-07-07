#pragma once

std::string removeHtmlTags(const std::string& htmlPage) {
	std::regex htmlTagTemplate("<.*?>");
	return std::regex_replace(htmlPage, htmlTagTemplate, "");
}