#include "memory.hpp"

#include <iostream>

int main()
{
	memory::process proc(L"notepad.exe");
	if (proc.open()) {
		wprintf(L"Succesfully opened %s (%d)\n", proc.name.c_str(), proc.id);

		for (const auto& address : proc.scan<int>(41)) {

		}
	}

	return 0;
}