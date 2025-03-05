#pragma once
#include <chrono>
#include <set>
#include <vector>

#include "databank.h"

namespace Databank
{
    struct PvEntry
    {
        int id = -1;
        int unk1 = 1;
        int unk2 = 0;
        std::string advStart, advEnd, start, end;

        bool operator<(const PvEntry& other) const
        {
            return id < other.id;
        }
    };

    class PvList
    {
    protected:
        std::set<PvEntry> entries;

    public:
        std::string finalString = std::string();
        //std::string checksum = std::string();

        bool readFromStr(std::string& str);
        bool readFromFile(const std::string& filename);
        bool finalize();
        void copyCstrData(char* dst);
        void addEntry(PvEntry& entry);
        void generateMissingEntries();
    };
}
