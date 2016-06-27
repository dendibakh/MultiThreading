#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

struct message {
    long mtype;
    char mtext[80];
};

int main()
{
	key_t key = ftok("/tmp/msg.temp", 1);
	int msgId = msgget(key, IPC_CREAT|IPC_EXCL|0777);

	struct msqid_ds ds;
	ds.msg_qbytes = 1024;
	msgctl(msgId, IPC_SET, &ds);

	message msg;
	memset(&msg, 0, sizeof(msg));
	ssize_t size = msgrcv(msgId, &msg, sizeof(msg) - sizeof(long), 0, MSG_NOERROR);
	std::string msgStr = ((char*)&(msg.mtext));

	std::cout << msgStr << '\n';

	{
		std::filebuf fb;
		fb.open ("/home/box/message.txt",std::ios::out);
		std::ostream os(&fb);
		os << msgStr;
		os.flush();
	}

	msgctl(msgId, IPC_RMID, 0);
	return 0;
}
