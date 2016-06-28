#include <fcntl.h>            /* Defines O_* constants */
#include <sys/stat.h>         /* Defines mode constants */
#include <mqueue.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

int main()
{
	mqd_t msgQId = mq_open("/test.mq", O_CREAT|O_RDONLY);

	struct mq_attr attr;
	mq_getattr(msgQId, &attr);

	char* buffer = new char[attr.mq_msgsize];
	memset(buffer, 0, attr.mq_msgsize);
	unsigned int prio;
	ssize_t size = mq_receive(msgQId, buffer, attr.mq_msgsize, &prio);
	std::string msgStr (buffer);

	std::cout << msgStr << '\n';

	{
		std::filebuf fb;
		fb.open ("/home/box/message.txt",std::ios::out);
		std::ostream os(&fb);
		os << msgStr;
		os.flush();
	}

	delete [] buffer;

	mq_close(msgQId);
	mq_unlink("/test.mq");
	return 0;
}
