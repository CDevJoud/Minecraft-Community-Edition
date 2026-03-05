#include <cstddef>
#include <cstdint>
#include <new>

// Delete corresponding to:
// void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
void __cdecl operator delete[](void* p, const char* name, int flags, unsigned debugFlags, const char* file, int line) noexcept {
	delete[] static_cast<uint8_t*>(p);
}

// Delete corresponding to:
// void* operator new[](uint64_t size, uint64_t alignment, uint64_t offset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
void __cdecl operator delete[](void* p, unsigned __int64 size, unsigned __int64 alignment, unsigned __int64 offset, const char* pName, int flags, unsigned debugFlags, const char* file, int line) noexcept {
	delete[] static_cast<uint8_t*>(p);
}

// You should also provide normal delete[] to match standard usage
void __cdecl operator delete[](void* p) noexcept {
	if (p != nullptr) {
		std::free(p);
		p = nullptr;
	}
}

void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line) {
	return new uint8_t[size];
}

void* __cdecl operator new[](unsigned __int64 size, unsigned __int64 alignment, unsigned __int64 offset, char const* pName, int flags, unsigned int debugFlags, char const* file, int line) {

	return new uint8_t[size];
}