#pragma once
#include <string>
#include <sstream>
#include <iomanip>

std::string urlDecode(const std::string& encoded)
{
    std::string decoded;
    char ch;
    for (size_t i = 0; i < encoded.length(); i++)
    {
        if (encoded[i] == '%')
        {
            if (i + 2 < encoded.length())
            {
                std::string hex = encoded.substr(i + 1, 2);
                ch = static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
                decoded.push_back(ch);
                i += 2;
            }
            else
            {
                decoded.push_back('%');
            }
        }
        else if (encoded[i] == '+')
        {
            decoded.push_back(' ');
        }
        else
        {
            decoded.push_back(encoded[i]);
        }
    }
    return decoded;
}

std::string urlEncode(const std::string& str)
{
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (unsigned char c : str)
    {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            escaped << c;
        }
        else
        {
            escaped << '%' << std::setw(2) << std::uppercase << static_cast<int>(c);
        }
    }
    return escaped.str();
}
