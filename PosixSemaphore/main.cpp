#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <unistd.h>

int main()
{
	sem_open("/test.sem", O_CREAT, 0777, 66);

	pause();

	return 0;
}
