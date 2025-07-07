#pragma once

#include <unordered_set>

#include "urlAddress.h"

std::unordered_set<UrlAddress> uniqueLinks(const std::vector<std::string>& rawLinks,
	ProtocolType protocol, const std::string& hostName) 
	{

	std::unordered_set<UrlAddress> result;


	for (const auto& l : rawLinks)
	{
		if (l[0] == '/')
		{
			result.insert({ ProtocolType::HTTPS, hostName, l });
		}
		else if ((l.substr(0, 7) == "http://") || (l.substr(0, 8) == "https://"))
		{
			result.insert(sharedAddress(l));
		}
	}
	return result;
}