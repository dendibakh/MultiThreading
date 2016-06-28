#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <unistd.h>

int main()
{
	sem_t * sem = sem_open("/test.sem", IPC_CREAT|IPC_EXCL, 0777, 66);

	pause();

	return 0;
}
