#pragma once

#include <string>

enum class ProtocolType
{
    HTTP = 0,
    HTTPS = 1
};

struct UrlAddress
{
    ProtocolType protocol;
    std::string hostName;
    std::string target;

    bool operator==(const UrlAddress& address) const
    {
        return protocol == address.protocol
               && hostName == address.hostName
               && target == address.target;
    }
};
