#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include <windows.h>

namespace StudioDumper
{
class Region
{
    std::vector<std::pair<uintptr_t, uintptr_t>> sections;

    static inline bool compare(const char* data_1, const char* data_2, const size_t size)
    {
        for (size_t i = 0; i < size; i++)
        {
            if (data_1[i] != data_2[i])
                return false;
        }
        return true;
    }

  public:
    static inline Region from_module(const uintptr_t base)
    {
        Region region;

        auto dos = (IMAGE_DOS_HEADER*) base;
        auto coff = (IMAGE_NT_HEADERS64*) (dos->e_lfanew + base);
        auto sections =
            (IMAGE_SECTION_HEADER*) ((uintptr_t) &coff->OptionalHeader + coff->FileHeader.SizeOfOptionalHeader);

        auto header_size = (uintptr_t) sections + sizeof(IMAGE_SECTION_HEADER) * coff->FileHeader.NumberOfSections;

        region.sections.push_back({base, header_size});

        for (size_t i = 0; i < coff->FileHeader.NumberOfSections; i++)
        {
            auto start = base + sections[i].VirtualAddress;
            auto end = base + sections[i].VirtualAddress + sections[i].Misc.VirtualSize;

            region.sections.push_back({start, end});
        }

        return region;
    }

    inline std::vector<uintptr_t> find(const std::string& match) const
    {
        std::vector<uintptr_t> matches;

        for (auto& section : this->sections)
        {
            for (auto i = section.first; i + match.size() < section.second; i++)
            {
                auto does_match = compare((char*) i, match.data(), match.size());

                if (does_match)
                    matches.push_back(i);
            }
        }

        return matches;
    }

    inline bool contains(const uintptr_t address) const
    {
        for (auto& section : this->sections)
        {
            if (section.first < address && address < section.second)
                return true;
        }

        return false;
    }

    inline uintptr_t base() const
    {
        if (this->sections.empty())
            return 0;

        uintptr_t smallest = this->sections[0].first;
        for (auto& section : this->sections)
        {
            if (section.second < smallest)
                smallest = section.first;
        }

        return smallest;
    }
};
} // namespace StudioDumper
