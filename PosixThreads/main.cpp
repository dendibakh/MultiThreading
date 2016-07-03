#include <pthread.h>
#include <iostream>     // std::cout, std::ostream, std::ios
#include <fstream>      // std::filebuf
#include <unistd.h>

void* start_routine(void *arg)
{
	int* intVal = (int*) arg;
	++(*intVal);
	return arg;
}

int main()
{
	{
		std::filebuf fb;
		fb.open ("/home/box/main.pid",std::ios::out);
		std::ostream os(&fb);
		pid_t pid = getpid();
		os << pid;
		os.flush();
	}

	pthread_t thread1;
	int val = 0;
	pthread_create(&thread1, NULL, start_routine, &val);

	pause();

	pthread_join(thread1, NULL);

	return 0;
}
