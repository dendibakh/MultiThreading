#include <pthread.h>
#include <iostream>     // std::cout, std::ostream, std::ios
#include <fstream>      // std::filebuf
#include <unistd.h>

pthread_mutex_t mp;
pthread_spinlock_t msp;
pthread_rwlock_t mrwp;

void* mutex_locker(void *arg)
{
	pthread_mutex_lock(&mp);
	pause();
	pthread_mutex_unlock(&mp);
	return arg;
}

void* spinlock_locker(void *arg)
{
	pthread_spin_lock(&msp);
	pause();
	pthread_spin_unlock(&msp);
	return arg;
}

void* wr_locker(void *arg)
{
	pthread_rwlock_wrlock(&mrwp);
	pause();
	pthread_rwlock_wrlock(&mrwp);
	return arg;
}

void* rd_locker(void *arg)
{
	pthread_rwlock_rdlock(&mrwp);
	pause();
	pthread_rwlock_rdlock(&mrwp);
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

	pthread_mutex_init(&mp, NULL);
	pthread_spin_init(&msp, PTHREAD_PROCESS_PRIVATE);
	pthread_rwlock_init(&mrwp, NULL);

	int val = 0;

	// mutex task
	pthread_t thread1;
	pthread_create(&thread1, NULL, mutex_locker, &val);

	sleep (1);

	// this thread will be blocked
	pthread_t thread2;
	pthread_create(&thread2, NULL, mutex_locker, &val);

	// spinlock task
	pthread_t thread3;
	pthread_create(&thread3, NULL, spinlock_locker, &val);

	sleep (1);

	// this thread will be blocked
	pthread_t thread4;
	pthread_create(&thread4, NULL, spinlock_locker, &val);

	// read-write lock task
	pthread_t thread5;
	pthread_create(&thread5, NULL, wr_locker, &val);

	sleep (1);

	// this thread will wait write lock
	pthread_t thread6;
	pthread_create(&thread6, NULL, wr_locker, &val);

	// this thread will wait read lock
	pthread_t thread7;
	pthread_create(&thread7, NULL, rd_locker, &val);

	pause();

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	pthread_join(thread4, NULL);
	pthread_join(thread5, NULL);
	pthread_join(thread6, NULL);
	pthread_join(thread7, NULL);

	return 0;
}
