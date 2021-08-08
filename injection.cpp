#include "memory.hpp"

bool memory::process::crt_inject(const std::wstring& dll)
{
	void* alloc = VirtualAllocEx(this->handle, NULL, dll.size(), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (alloc == NULL) {
		perror("[ERROR]: Failed to allocate memory.");
		return false;
	}
}

bool memory::process::inject(const std::wstring& dll, memory::injection_method method)
{
	switch (method)
	{
	case memory::injection_method::CRT:
		return this->crt_inject(dll);
	default:
		break;
	}

	return false;
}