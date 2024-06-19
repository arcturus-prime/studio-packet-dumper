#pragma once

#include "Region.h"

#include <cstdint>
#include <optional>
#include <string>
#include <windows.h>

namespace VFTable {
	struct Context {
		std::vector<uintptr_t> hooks;

		void** vftable;

		static inline std::optional<Context> find(Region& region, const std::string& mangled_name) {
			auto results = region.find(mangled_name);
			auto base = region.base();

			for (auto result: results)
			{
				unsigned int ibo_type_descriptor = result - base - 0x10;

				auto xref_ibos = region.find(std::string((char*) &ibo_type_descriptor, 4));

				for (auto xref_ibo: xref_ibos)
				{
					uintptr_t completeObjLoc = xref_ibo - 0xC;
					auto vftable = region.find(std::string((char*) &completeObjLoc, 8));

					if (vftable.empty())
						return std::nullopt;

					size_t i = 0;
					while (region.contains(*((uintptr_t*) vftable[0] + i + 1))) i++;

					return Context{std::vector<uintptr_t>(i), (void**) vftable[0] + 1};
				}
			}

			return std::nullopt;
		}

		inline void hook(size_t position, void* function) {
			DWORD oldSetting;
			VirtualProtect((void*) (vftable + position), 8, PAGE_READWRITE, (PDWORD) &oldSetting);

			this->hooks[position] = (uintptr_t) * (vftable + position);
			*(vftable + position) = function;

			VirtualProtect((void*) (vftable + position), 8, oldSetting, (PDWORD) &oldSetting);
		}

		inline void unhook(size_t index) {
			DWORD oldSetting;
			VirtualProtect((void*) (vftable + index), 8, PAGE_READWRITE, (PDWORD) &oldSetting);

			*(vftable + index) = (void*) this->hooks[index];

			VirtualProtect((void*) (vftable + index), 8, oldSetting, (PDWORD) &oldSetting);
		}

		inline uintptr_t get_current(size_t position) {
			return *(uintptr_t*) (vftable + position);
		}

		inline uintptr_t get_previous(size_t position) {
			return this->hooks[position];
		}
	};
} // namespace VFTable