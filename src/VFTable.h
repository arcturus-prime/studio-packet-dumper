#pragma once

#include "Context.h"
#include "Region.h"
#include "Types.h"

#include <cstdint>
#include <iostream>
#include <windows.h>

namespace VFTable
{
	static inline void** find(Region& region, const std::string& match)
	{
		auto results = region.find(match);
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
					return NULL;

				return (void**) vftable[0];
			}
		}

		return NULL;
	}
} // namespace VFTable