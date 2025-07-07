#pragma once

#include "urlAddress.h"

UrlAddress sharedAddress(const std::string& address) {

	UrlAddress resultAddress;

	if (address.find("https") != std::string::npos) {
		resultAddress.protocol = ProtocolType::HTTPS;
	}
	else {
		resultAddress.protocol = ProtocolType::HTTP;
	}

	size_t posDoubleSlesh = address.find("//");
	std::string strAfterDoubleSlesh = address.substr(posDoubleSlesh + 2);

	if (strAfterDoubleSlesh.find("/") == std::string::npos) {
		resultAddress.hostName = strAfterDoubleSlesh.substr(0);
		resultAddress.target = "/";
	}
	else {
		size_t posSlesh = strAfterDoubleSlesh.find("/");
		resultAddress.hostName = strAfterDoubleSlesh.substr(0, posSlesh);
		resultAddress.target = strAfterDoubleSlesh.substr(posSlesh);
	}

	return resultAddress;
}