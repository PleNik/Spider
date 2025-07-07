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

namespace std {

    template <>
    struct hash<UrlAddress> {
        std::size_t operator()(const UrlAddress& address) const noexcept {
            std::size_t h1 = std::hash<int>{}(static_cast<int>(address.protocol));
            std::size_t h2 = std::hash<std::string>{}(address.hostName);
            std::size_t h3 = std::hash<std::string>{}(address.target);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

}
