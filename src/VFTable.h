#pragma once

#include "Region.h"

#include <cstdint>
#include <optional>
#include <string>
#include <windows.h>

namespace StudioDumper
{
class VFTable
{
    std::vector<uintptr_t> hooks;
    uintptr_t* vftable;

  public:
    VFTable() = default;

    static inline std::optional<VFTable> find(const Region& region, const std::string& mangled_name)
    {
        auto results = region.find(mangled_name);
        auto base = region.base();

        for (auto result : results)
        {
            unsigned int ibo_type_descriptor = result - base - 0x10;

            auto xref_ibos = region.find(std::string((char*) &ibo_type_descriptor, 4));

            for (auto xref_ibo : xref_ibos)
            {
                uintptr_t completeObjLoc = xref_ibo - 0xC;
                auto vftable_results = region.find(std::string((char*) &completeObjLoc, 8));

                if (vftable_results.empty())
                    return std::nullopt;

                auto vftable = (uintptr_t*) vftable_results[0] + 1;

                size_t i = 0;
                while (region.contains(vftable[i]))
                    i++;

                VFTable ctx;

                ctx.hooks = std::vector<uintptr_t>(i);
                ctx.vftable = vftable;

                return ctx;
            };
        }

        return std::nullopt;
    }

    inline void hook(size_t position, uintptr_t function)
    {
        if (position >= this->hooks.size())
            return;

        DWORD oldSetting;
        VirtualProtect((void*) (this->vftable + position), 8, PAGE_READWRITE, (PDWORD) &oldSetting);

        this->hooks[position] = *(this->vftable + position);
        *(this->vftable + position) = function;

        VirtualProtect((void*) (this->vftable + position), 8, oldSetting, (PDWORD) &oldSetting);
    }

    inline void unhook(size_t position)
    {
        if (position >= this->hooks.size())
            return;

        DWORD oldSetting;
        VirtualProtect((void*) (this->vftable + position), 8, PAGE_READWRITE, (PDWORD) &oldSetting);

        *(this->vftable + position) = this->hooks[position];

        VirtualProtect((void*) (this->vftable + position), 8, oldSetting, (PDWORD) &oldSetting);
    }

    inline uintptr_t get_current(size_t position)
    {
        if (position >= this->hooks.size())
            return 0;

        return *(this->vftable + position);
    }

    inline uintptr_t get_previous(size_t position)
    {
        if (position >= this->hooks.size())
            return 0;

        return this->hooks[position];
    }

    inline uintptr_t get_address()
    {
        return (uintptr_t) this->vftable;
    }

    inline size_t get_size()
    {
        return this->hooks.size();
    }
};
} // namespace StudioDumper