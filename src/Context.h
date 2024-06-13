#pragma once

#include <cstdint>
#include <cstring>
#include <utility>
#include <vector>
#include <windows.h>

struct HookTableEntry {
	uintptr_t address;
	std::vector<uint8_t> previous;
};

struct Context {
	std::vector<HookTableEntry> hooks;
	std::vector<size_t> holes;

	inline size_t hook(uintptr_t address, char* data, size_t size)
	{
		DWORD oldSetting;
		VirtualProtect((void*) address, size, PAGE_READWRITE, (PDWORD) &oldSetting);

		HookTableEntry entry;

		entry.previous = std::vector<uint8_t>((char*) address, (char*) address + size);
		entry.address = address;

		memcpy((void*) address, data, size);

		VirtualProtect((void*) address, size, oldSetting, (PDWORD) &oldSetting);

		if (this->holes.size() > 0)
		{
			auto hole = this->holes[this->holes.size() - 1];
			this->holes.pop_back();

			this->hooks[hole] = entry;
			return hole;
		} else
		{
			this->hooks.push_back(entry);
			return this->hooks.size() - 1;
		}
	}
	inline void unhook(size_t index)
	{
		auto entry = &this->hooks[index];

		DWORD oldSetting;
		VirtualProtect((void*) entry->address, entry->previous.size(), PAGE_READWRITE, (PDWORD) &oldSetting);

		memcpy((void*) entry->address, entry->previous.data(), entry->previous.size());

		VirtualProtect((void*) entry->address, entry->previous.size(), oldSetting, (PDWORD) &oldSetting);

		this->holes.push_back(index);
	}
};