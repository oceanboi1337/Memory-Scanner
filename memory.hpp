#ifndef _H_MEMORY
#define _H_MEMORY

#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

#include <string>
#include <vector>

namespace memory
{
	typedef struct memory_region {
		uintptr_t start;
		uintptr_t end;
		uintptr_t size;
	} memory_region;

	typedef struct module {
		std::wstring name;
		uintptr_t start;
		uintptr_t end;
		uintptr_t size;
	} module;

	enum injection_method { CRT, MANUAL };

	class process
	{
	private:
		bool crt_inject(const std::wstring& dll);

	public:
		std::wstring name;
		int id;
		void* handle;

		process(const std::wstring& name);

		bool open(unsigned long access_level = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE);
		bool inject(const std::wstring& dll, injection_method method);

		std::vector<memory_region> memory_regions();
		std::vector<module> modules();

		template <class T>
		inline T read(uintptr_t address, size_t size, size_t* bytes_read = NULL)
		{
			T buffer;
			ReadProcessMemory(this->handle, reinterpret_cast<void*>(address), &buffer, size, bytes_read);
			return buffer;
		}

		template <class T>
		inline size_t write(uintptr_t address, T value)
		{
			size_t bytes_written;
			WriteProcessMemory(this->handle, reinterpret_cast<void*>(address), &value, sizeof(T), &bytes_written);
			return bytes_written;
		}

		template <class T>
		inline std::vector<uintptr_t> scan(T value)
		{
			std::vector<uintptr_t> results;

			for (const auto& region : this->memory_regions()) {
				unsigned char* buffer = (unsigned char*)malloc(region.size);
				if (buffer == NULL) continue;

				memset(buffer, 0, region.size);
				ReadProcessMemory(this->handle, (void*)region.start, buffer, region.size, NULL);

				size_t size = sizeof(T);

				for (int i = 0; i < region.size; i += size) {
					if (memcmp(buffer + i, &value, size) == 0) {
						results.push_back(region.start + i);
					}
				}
			}

			return results;
		}

	};

	inline process::process(const std::wstring& process_name)
	{
		this->name = process_name;
		this->id = 0;
	}
}

#endif