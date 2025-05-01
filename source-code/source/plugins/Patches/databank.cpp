#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

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
                std::tuple<int, int> id(std::stoi(tokens[i]), std::stoi(tokens[i + 2]));
                entry.unk1 = std::stoi(tokens[i + 1]);
                entry.advStart = tokens[i + 3];
                entry.advEnd = tokens[i + 4];
                entry.start = tokens[i + 5];
                entry.end = tokens[i + 6];
                entries.insert({ id, entry });
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

    void PvList::addEntry(int id, int extra, PvEntry& entry)
    {
        entries.insert({ std::tuple<int, int>(id, extra), entry });
    }

    void PvList::generateMissingEntries()
    {
        PvEntry entry = PvEntry();

        for (int i = 0; i <= 999; i++)
        {
            entry.advStart = "2000-01-01";
            entry.advEnd = "2029-01-01";
            entry.start = "2000-01-01";
            entry.end = "2029-01-01";

            entries.insert({ std::tuple<int, int>(i, 0), entry });
            entries.insert({ std::tuple<int, int>(i, 1), entry });
        }
    }

    std::string PvList::finalize()
    {
        std::ostringstream csvStream;
        if (entries.empty())
        {
            return "%2A%2A%2A"; // ***
            //checksum = "5341";
        }
        else
        {
            bool first = true;

            for (const auto& entryMap : entries)
            {
                std::ostringstream entryStream;
                if (!first)
                {
                    csvStream << ",";
                }
                first = false;
                const PvEntry& entry = entryMap.second;
                entryStream << std::get<0>(entryMap.first) << ","
                    << entry.unk1 << ","
                    << std::get<1>(entryMap.first) << ","
                    << entry.advStart << ","
                    << entry.advEnd << ","
                    << entry.start << ","
                    << entry.end;
                csvStream << urlEncode(entryStream.str());
            }

            return urlEncode(csvStream.str());
            //checksum = std::to_string(crc16_ccitt(finalString));
        }
    }
}
