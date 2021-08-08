# MemoryThing
Second biggest cope on github

```cpp
#include "memory.hpp"

#include <iostream>

int main()
{
	memory::process proc(L"LeagueClientUx.exe");
	if (proc.open()) {
		wprintf(L"Succesfully opened %s (%d)\n", proc.name.c_str(), proc.id);

		for (const auto& address : proc.scan<int>(7093)) {
			proc.write<int>(address, 10000);
			std::wcout << "0x" << std::hex << address << std::endl;
		}
	}

	return 0;
}
```
