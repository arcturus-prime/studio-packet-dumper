#pragma once

#include "Types.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

struct Section {
	uintptr_t start;
	uintptr_t end;
};

struct Region {
	std::vector<Section> sections;

	static inline Region fromModule(const void* base)
	{
		Region region;

		IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*) base;
		IMAGE_NT_HEADERS64* coff =
			(IMAGE_NT_HEADERS64*) (dos->e_lfanew + (uintptr_t) base);
		IMAGE_SECTION_HEADER* sections =
			(IMAGE_SECTION_HEADER*) ((uintptr_t) &coff->OptionalHeader +
									 coff->FileHeader.SizeOfOptionalHeader);

		region.sections.push_back({(uintptr_t) base, (uintptr_t) sections + sizeof(IMAGE_SECTION_HEADER) * coff->FileHeader.NumberOfSections});

		for (size_t i = 0; i < coff->FileHeader.NumberOfSections; i++)
		{
			region.sections.push_back({(uintptr_t) base + sections[i].VirtualAddress, (uintptr_t) base + sections[i].VirtualAddress + sections[i].Misc.VirtualSize});
		}

		return region;
	}

	inline std::vector<uintptr_t> find(const std::string& match) const
	{
		std::vector<uintptr_t> matches;

		for (auto& section: this->sections)
		{
			for (uintptr_t i = section.start; i + match.size() < section.end; i++)
			{
				if (compare((char*) i, match.data(), match.size()))
					matches.push_back(i);
			}
		}

		return matches;
	}

	inline void merge(Region& region)
	{
		this->sections.insert(this->sections.end(), region.sections.begin(), region.sections.end());
	}

	inline bool contains(const uintptr_t address) const
	{
		for (auto& section: this->sections)
		{
			if (section.start < address && address < section.end)
				return true;
		}

		return false;
	}

	inline uintptr_t base() const
	{
		if (this->sections.empty())
			return 0;

		uintptr_t smallest = this->sections[0].start;
		for (auto& section: this->sections)
		{
			if (section.start < smallest)
				smallest = section.start;
		}

		return smallest;
	}

private:
	Region() = default;

	static inline bool compare(const char* data1, const char* data2, const size_t size)
	{
		for (size_t i = 0; i < size; i++)
		{
			if (data1[i] != data2[i])
				return false;
		}
		return true;
	}
};