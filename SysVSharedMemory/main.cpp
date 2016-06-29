#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

int main()
{
	key_t key = ftok("/tmp/mem.temp", 1);

	int shmId = shmget(key, 1024*1024*1024, IPC_CREAT);
	if (shmId == -1)
	{
		std::cout << "Shared memory was not created.\n";
		return 1;
	}

	void* allocShMem = shmat(shmId, NULL, 0);
	if (allocShMem == (void *) -1)
	{
		std::cout << "Shared memory was not attached.\n";
		return 1;
	}

	memset(allocShMem, 42, 1024*1024*1024);

	pause();

	shmdt(allocShMem);

	shmctl(shmId, IPC_RMID, NULL);

	return 0;
}
