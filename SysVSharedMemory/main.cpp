#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <errno.h>
#include <sys/stat.h>

int main()
{
	key_t key = ftok("/tmp/mem.temp", 1);

	size_t size = (1024*1024*1024 / getpagesize() + 1) * getpagesize();
	std::cout << "Shared memory size = " << size << ".\n";

	int shmId = shmget(key, size, IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR|S_IROTH|S_IWOTH);
	if (shmId == -1)
	{
		std::cout << "Shared memory was not created.\n";
		std::cout << "errno = " << errno << ".\n";
		return 1;
	}

	void* allocShMem = shmat(shmId, 0, 0);
	if (allocShMem == (void *) -1)
	{
		std::cout << "Shared memory was not attached.\n";
		std::cout << "errno = " << errno << ".\n";
		shmctl(shmId, IPC_RMID, 0);
		return 1;
	}

	memset(allocShMem, 42, size);

	pause();

	shmdt(allocShMem);

	shmctl(shmId, IPC_RMID, 0);

	return 0;
}
