#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

int main()
{
	key_t key = ftok("/tmp/sem.temp", 1);

	int semId = semget(key, 16, IPC_CREAT|IPC_EXCL|0777);

	for (int i = 0; i < 16; ++i)
	{
		semctl(semId, i, SETVAL, i);
	}

	pause();

	return 0;
}
