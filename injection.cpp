#include "memory.hpp"

bool memory::process::crt_inject(const std::wstring& dll)
{
	void* load_library = (void*)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
	if (load_library == NULL) {
		perror("[ERROR]: Failed to find LoadLibraryA address.");
		return false;
	}

	void* alloc = VirtualAllocEx(this->handle, NULL, dll.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (alloc == NULL) {
		perror("[ERROR]: Failed to allocate memory.");
		return false;
	}

	int bytes_written = WriteProcessMemory(this->handle, alloc, dll.data(), dll.size(), NULL);
	if (bytes_written == 0) {
		perror("[ERROR]: Failed to write dll path.");
	}

	void* thread = CreateRemoteThread(this->handle, NULL, 0, (LPTHREAD_START_ROUTINE)load_library, alloc, NULL, NULL);
	if (thread == NULL) {
		perror("[ERROR]: CreateRemoteThread failed.");
		return false;
	}

	return true;
}

bool memory::process::inject(const std::wstring& dll, memory::injection_method method)
{
	switch (method)
	{
	case memory::injection_method::CRT:
		return this->crt_inject(dll);
	default:
		return false;
	}

	return false;
}