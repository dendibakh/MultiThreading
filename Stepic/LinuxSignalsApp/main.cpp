#include <signal.h>
#include <iostream>     // std::cout, std::ostream, std::ios
#include <fstream>      // std::filebuf
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

int main()
{
	int childPid = fork();
	if (childPid)
	{
		{
			std::filebuf fb;
			fb.open ("/home/denis/pid_parent",std::ios::out);
			std::ostream os(&fb);
			pid_t pid = getpid();
			os << pid;
			os.flush();
		}

		{
			std::filebuf fb;
			fb.open ("/home/denis/pid_child",std::ios::out);
			std::ostream os(&fb);
			pid_t pid = childPid;
			os << pid;
			os.flush();
		}

		signal(SIGTERM, SIG_IGN);
	}
	signal(SIGINT, SIG_IGN);

	if (childPid)
	{
		int status = 0;
		waitpid(childPid, &status, 0);
		std::cout << "Process " << childPid << "ends with exit-code = " << status << std::endl;
	}

	pause();
	return 0;
}
