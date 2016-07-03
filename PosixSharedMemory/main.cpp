#include <fcntl.h>            /* Defines O_* constants */
#include <sys/stat.h>         /* Defines mode constants */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <cstring>
#include <iostream>

int main()
{
	int shmId = shm_open("/test.shm", O_CREAT | O_RDWR, 0777);
	if (shmId == -1)
	{
		std::cout << "Error while creating a shared memory.\n";
		return 1;
	}

	size_t size = (1024*1024 / getpagesize() + 1) * getpagesize();
	std::cout << "Shared memory size = " << size << ".\n";

	ftruncate(shmId, size + 1);

	char* allocShMem = (char*)mmap(0, size + 1, PROT_WRITE|PROT_READ, MAP_SHARED, shmId, 0);
	if (allocShMem == (void *) -1)
	{
		std::cout << "Shared memory was not attached.\n";
		std::cout << "errno = " << errno << ".\n";
		shm_unlink("/test.shm");
		return 1;
	}

	// fill all bytes with 13
	memset(allocShMem, 13, size);

	pause();

	munmap(allocShMem, size);
	close(shmId);
	shm_unlink("/test.shm");
	return 0;
}
