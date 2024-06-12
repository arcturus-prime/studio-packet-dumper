#pragma once

#include "Types.h"

#include <cstdint>
#include <string>
#include <vector>
#include <windows.h>

struct Section {
	uintptr_t start;
	uintptr_t end;
};

struct Region {
	std::vector<Section> sections;

	Region(void* base)
	{
		IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*) base;
		IMAGE_NT_HEADERS64* coff =
			(IMAGE_NT_HEADERS64*) (dos->e_lfanew + (uintptr_t) base);
		IMAGE_SECTION_HEADER* sections =
			(IMAGE_SECTION_HEADER*) ((uintptr_t) &coff->OptionalHeader +
									 coff->FileHeader.SizeOfOptionalHeader);

		this->sections.reserve(coff->FileHeader.NumberOfSections);
		for (int i = 0; i < coff->FileHeader.NumberOfSections; i++)
		{
			this->sections.push_back({(uintptr_t) base + sections[i].VirtualAddress, sections[i].VirtualAddress + sections[i].Misc.VirtualSize});
		}
	}

	inline std::vector<uintptr_t> find(std::string& match)
	{
		std::vector<uintptr_t> matches;

		for (auto& section: this->sections)
		{
			for (uintptr_t i = section.start; i < section.end; i++)
			{
				for (size_t j = 0; j < match.size() && i + j < section.end; j++)
				{
					if (*(char*) i + j != match[j])
						goto CONTINUE;
				}

				matches.push_back(i);
			CONTINUE:
				continue;
			}
		}

		return matches;
	}

	inline void merge(Region& region)
	{
		this->sections.insert(this->sections.end(), region.sections.begin(), region.sections.end());
	}

	inline bool contains(uintptr_t address)
	{
		for (auto& section: this->sections)
		{
			if (section.start < address && address < section.end)
				return true;
		}

		return false;
	}
};