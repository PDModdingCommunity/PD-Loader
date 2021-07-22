#pragma once
void InjectCode(void* address, const std::vector<uint8_t> data);
void ApplyPatches();
void ApplyAllCustomPatches();
void ApplyCustomPatches(std::wstring CPATCH_FILE_STRING);
