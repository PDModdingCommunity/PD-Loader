#include <iostream>
#include <fstream>
#include <sstream>

#include "databank.h"
#include "urlutil.h"

namespace Databank
{
    bool PvList::readFromStr(std::string& str)
    {
        std::string decoded = urlDecode(urlDecode(str));

        std::vector<std::string> tokens;
        std::stringstream ss(decoded);
        std::string token;
        while (std::getline(ss, token, ','))
        {
            if (!token.empty())
            {
                tokens.push_back(token);
            }
        }

        if (tokens.size() % 7 != 0)
        {
            std::cerr << "[Patches] Databank error: Token count (" << tokens.size() << ") is not a multiple of 7." << std::endl;
            return false;
        }

        for (size_t i = 0; i < tokens.size(); i += 7)
        {
            try
            {
                PvEntry entry;
                entry.id = std::stoi(tokens[i]);
                entry.unk1 = std::stoi(tokens[i + 1]);
                entry.unk2 = std::stoi(tokens[i + 2]);
                entry.advStart = tokens[i + 3];
                entry.advEnd = tokens[i + 4];
                entry.start = tokens[i + 5];
                entry.end = tokens[i + 6];
                entries.insert(entry);
            }
            catch (const std::exception& e)
            {
                std::cerr << "[Patches] Databank parse error: " << e.what() << std::endl;
                return false;
            }
        }
        return true;
    }

    bool PvList::readFromFile(const std::string& filename)
    {
        try
        {
            std::ifstream file(filename, std::ios::in | std::ios::binary);
            if (!file)
            {
                std::cerr << "[Patches] Failed to open databank " + filename << std::endl;
                return false;
            }

            std::string contents = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            return readFromStr(contents);
        }
        catch (const std::exception& e)
        {
            std::cerr << "[Patches] Databank file read error: " << e.what() << std::endl;
            return false;
        }
    }

    bool PvList::finalize()
    {
        std::ostringstream csvStream;
        if (entries.empty())
        {
            finalString = "%2A%2A%2A"; // ***
            //checksum = "5341";
        }
        else
        {
            bool first = true;

            for (const auto& entry : entries)
            {
                std::ostringstream entryStream;
                if (!first)
                {
                    csvStream << ",";
                }
                first = false;
                entryStream << entry.id << ","
                    << entry.unk1 << ","
                    << entry.unk2 << ","
                    << entry.advStart << ","
                    << entry.advEnd << ","
                    << entry.start << ","
                    << entry.end;
                csvStream << urlEncode(entryStream.str());
            }

            finalString = urlEncode(csvStream.str());
            //checksum = std::to_string(crc16_ccitt(finalString));
        }
        return true;
    }

    void PvList::copyCstrData(char* dst)
    {
        strcpy_s((char*)dst, finalString.size(), finalString.c_str());
    }

    void PvList::addEntry(PvEntry& entry)
    {
        entries.insert(entry);
    }

    void PvList::generateMissingEntries()
    {
        PvEntry entry = PvEntry();

        for (int i = 0; i <= 999; i++)
        {
            entry.id = i;
            entry.advStart = "2000-01-01";
            entry.advEnd = "2029-01-01";
            entry.start = "2000-01-01";
            entry.end = "2029-01-01";

            entries.insert(entry);
        }
    }
}
