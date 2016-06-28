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
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 80;
    attr.mq_curmsgs = 0;

    mqd_t msgQId = mq_open( "/test.mq", O_CREAT | O_RDWR, 0777, &attr );
	if (msgQId == -1)
	{
		std::cout << "Error while creating a message Queue.\n";
		return 1;
	}

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
