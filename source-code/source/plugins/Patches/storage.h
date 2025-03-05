#pragma once
#include <cstdint>

namespace Storage
{
    struct FileRecord
    {
        union
        {
            void* pData;          // used if allocSize is >= 16
            char  inlineData[16]; // used if allocSize is < 16; [8-15] not normally cleared when expanded
        };
        uint64_t dataLength = 0;
        uint64_t allocSize = 15;
    };

    //void(__fastcall* divaAddStringContainer)(void*, void**) = (void(__fastcall*)(void* container, void** src))0x01400C0A40;

    void customAddStringContainer(void* container, const char* str)
    {
        uint64_t addr = (uint64_t)container;

        while (*(uint64_t*)addr != 0)
        {
            addr += sizeof(FileRecord);
        }

        FileRecord* record = (FileRecord*)addr;
        size_t length = strlen(str) + 1;

        if (length > 0 && length < 16)
        {
            memcpy_s(record->inlineData, length, str, length);
            record->dataLength = length;
            record->allocSize = 0;
        }
        else if (length >= 16)
        {
            char* alloc = (char*)malloc(length);
            memcpy_s(alloc, length, str, length);
            record->pData = alloc;
            record->dataLength = length;
            record->allocSize = length;
        }
    }
};