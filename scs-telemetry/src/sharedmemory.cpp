#include "sharedmemory.hpp"
#include "sharedmemory_platform.hpp"
#include <cstring>

void SharedMemory::LogError(const char* logPtr) {
#ifdef SHAREDMEM_LOGGING
	if (this->logFilePtr == NULL)
	{
		this->logFilePtr = fopen(SHAREDMEM_FILENAME, "a");
	}
	if (this->logFilePtr != NULL)
	{
		fprintf(this->logFilePtr, "%s\r\n", logPtr);
		fprintf(this->logFilePtr, "Error code: %d\r\n\r\n", SharedMemoryPlatform::get_last_error());
	}
#endif
}

SharedMemory::SharedMemory(const char* newNamePtr, unsigned int size) {
    this->mapsize = size;
    this->namePtr = SCS_SHM_NAME;
    this->isSharedMemoryHooked = false;
#ifdef SHAREDMEM_LOGGING
	this->logFilePtr = NULL;
#endif

    hMapFile = SharedMemoryPlatform::create_or_open(this->namePtr, size);
    LogError("Created file map");

    if (hMapFile == SCS_INVALID_HANDLE) {
        LogError("Could not create or open shared memory");
        return;
    }

    this->pBufferPtr = SharedMemoryPlatform::map(hMapFile, size);
    if (this->pBufferPtr == SCS_MAP_FAILED) {
        LogError("Could not reserve buffer for shared memory");
        SharedMemoryPlatform::close(hMapFile);
    }
    else {
        memset(this->pBufferPtr, 0, size);
        this->isSharedMemoryHooked = true;
        LogError("Opened MMF");
    }
}


void SharedMemory::Close() {
#ifdef SHAREDMEM_LOGGING
		if (logFilePtr != NULL)
		{
			fclose(logFilePtr);
			// TODO: Is this closed properly?
		}
#endif
    if (isSharedMemoryHooked) {
        if (pBufferPtr != SCS_MAP_FAILED) {
            SharedMemoryPlatform::unmap(pBufferPtr, mapsize);
        }
        SharedMemoryPlatform::close(hMapFile);
    }

    isSharedMemoryHooked = false;

}
