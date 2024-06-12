#pragma once

#include "Types.h"
#include "Utility.h"

#include <windows.h>

static inline void* VFTable_hook(void** vftablePtr, int position, void* func)
{
	int oldSetting;
	VirtualProtect(vftablePtr, position * sizeof(void*), PAGE_READWRITE, (PDWORD) &oldSetting);

	void* oldFunc = vftablePtr[position];
	vftablePtr[position] = func;

	VirtualProtect(vftablePtr, position * sizeof(void*), oldSetting, (PDWORD) &oldSetting);

	return oldFunc;
}

static inline void** VFTable_find(void* base, const char* mangled_name, size_t size)
{
	char* result = Utility_findInImage(base, mangled_name, size);
	if (result == NULL)
		return NULL;

	unsigned int ibo_type_descriptor = (uintptr_t) result - (uintptr_t) base - 0x10;

	char* completeObjLoc =
		Utility_findInImage(base, (char*) &ibo_type_descriptor, 4) - 0xC;

	char* vftable = Utility_findInImage(base, (char*) &completeObjLoc, 8) + 8;

	return (void**) vftable;
}