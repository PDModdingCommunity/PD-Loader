#pragma once
#include <map>

#include "databank.h"

namespace Databank
{
    struct PvEntry
    {
        int unk1 = 1;
        int unk2 = 0;
        std::string advStart, advEnd, start, end;
    };

    class PvList
    {
    protected:
        std::map<int, PvEntry> entries;

    public:
        bool readFromStr(std::string& str);
        bool readFromFile(const std::string& filename);
        void addEntry(int id, PvEntry& entry);
        void generateMissingEntries();
        std::string finalize();
    };
}
