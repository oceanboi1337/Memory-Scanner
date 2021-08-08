#include "memory.hpp"

#include <tchar.h> 
#include <iostream>

std::vector<memory::module> memory::process::modules()
{
	std::vector<memory::module> modules;

	HANDLE snapshot = INVALID_HANDLE_VALUE;
	MODULEENTRY32 entry;

	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->id);
	if (snapshot == INVALID_HANDLE_VALUE) {
		perror("[ERROR]: Failed to snapshot modules.");
		return modules;
	}

	entry.dwSize = sizeof(MODULEENTRY32);

	if (!Module32First(snapshot, &entry)) {
		perror("[ERROR]: Failed to get first module.");
		CloseHandle(snapshot);
		return modules;
	}

	do {
		memory::module mod;
		mod.name = std::wstring(entry.szModule);
		mod.start = (uintptr_t)entry.modBaseAddr;
		mod.end = (uintptr_t)entry.modBaseAddr + entry.modBaseSize;
		mod.size = entry.modBaseSize;

		modules.push_back(mod);

	} while (Module32Next(snapshot, &entry));

	CloseHandle(snapshot);

	return modules;
}

bool memory::process::open(unsigned long access_level)
{
	HANDLE snapshot = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 entry;

	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == snapshot) {
		return false;
	}

	entry.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(snapshot, &entry)) {
		CloseHandle(snapshot);
		return false;
	}

	do {
		if (!std::wstring(this->name.begin(), this->name.end()).compare(entry.szExeFile)) {
			this->id = entry.th32ProcessID;
			this->handle = OpenProcess(access_level, NULL, this->id);

			CloseHandle(snapshot);
			return true;
		}
	} while (Process32Next(snapshot, &entry));

	return false;
}

std::vector<memory::memory_region> memory::process::memory_regions()
{
	std::vector<memory::memory_region> regions;
	MEMORY_BASIC_INFORMATION info;

	uintptr_t address = 0;
	while (VirtualQueryEx(this->handle, (void*)address, &info, sizeof(info))) {
		if (info.State == MEM_COMMIT && info.Type == MEM_PRIVATE && info.Protect == PAGE_READWRITE) {
			memory_region region;
			region.start = (uintptr_t)info.BaseAddress;
			region.end = (uintptr_t)info.BaseAddress + info.RegionSize;
			region.size = info.RegionSize;

			regions.push_back(region);
		}
		address += info.RegionSize;
	}

	return regions;
}