#pragma once
#include <map>

#include "databank.h"

namespace Databank
{
    struct PvEntry
    {
        int unk1 = 1;
        std::string advStart, advEnd, start, end;
    };

    class PvList
    {
    protected:
        std::map<std::tuple<int, int>, PvEntry> entries;

    public:
        bool readFromStr(std::string& str);
        bool readFromFile(const std::string& filename);
        void addEntry(int id, int extra, PvEntry& entry);
        void generateMissingEntries();
        std::string finalize();
    };
}
