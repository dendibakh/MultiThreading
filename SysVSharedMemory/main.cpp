#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <cstring>

int main()
{
	key_t key = ftok("/tmp/mem.temp", 1);

	int shmId = shmget(key, 1024*1024*1024, IPC_CREAT);

	void* allocShMem = shmat(shmId, NULL, 0);

	memset(allocShMem, 42, 1024*1024*1024);

	pause();

	shmdt(allocShMem);

	shmctl(shmId, IPC_RMID, NULL);

	return 0;
}
