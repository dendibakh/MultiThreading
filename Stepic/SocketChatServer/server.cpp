/*
 * server.cpp
 *
 *  Created on: Jun 7, 2016
 *      Author: bakhvalo
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <set>
#include <iostream>
#include <algorithm>
#include <list>

// This function is portable for setting socket in non-blocking mode
#include <sys/ioctl.h>
#include <sys/fcntl.h>

int set_nonblock(int fd)
{
	int flags;
	#if defined(O_NONBLOCK)
		if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
			flags = 0;
		return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	#else
		flags = 1;
		return ioctl(fd, FIOBIO, &flags);
	#endif
}

void sendToEverybody(const std::string& msg, const std::set<int>& SlaveSockets)
{
	std::for_each(SlaveSockets.begin(), SlaveSockets.end(), [&] (int slaveSocket)
							{
								send(slaveSocket, (void*)msg.data(), msg.size(), MSG_NOSIGNAL);
							});
}

int main()
{
	int masterSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (masterSocket == -1)
		std::cout << "Failed to open socket" << std::endl;

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(12345);
	sa.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0

	if (bind(masterSocket, (sockaddr*)&sa, sizeof(sa)) == -1)
			std::cout << "Failed to bind socket" << std::endl;

	if (set_nonblock(masterSocket) == -1)
			std::cout << "Failed to set master socket as non-block" << std::endl;

	if (listen(masterSocket, SOMAXCONN) == -1)
			std::cout << "Failed to listen to socket" << std::endl;

	std::set<int> SlaveSockets;

	while (true)
	{
		fd_set Set;
		FD_ZERO(&Set);
		FD_SET(masterSocket, &Set);
		std::for_each(SlaveSockets.begin(), SlaveSockets.end(), [&] (int slaveSocket)
						{
							FD_SET(slaveSocket, &Set);
						});

		auto iter = std::max_element(SlaveSockets.begin(), SlaveSockets.end());
		int max = masterSocket;
		if (iter != SlaveSockets.end())
			max = std::max(masterSocket, *iter);

		select(max + 1, &Set, NULL, NULL, NULL);

		// process slave sockets
		// the case when some socket is closed or here is some data on the socket
		std::list<int> socketsToRemove;
		std::for_each(SlaveSockets.begin(), SlaveSockets.end(), [&] (int slaveSocket)
				{
					if (FD_ISSET(slaveSocket, &Set))
					{
						static char buff[1024];
						int recvSize = recv(slaveSocket, buff, 1024, MSG_NOSIGNAL);
						if ((recvSize == 0) && (errno != EAGAIN))
						{
							shutdown(slaveSocket, SHUT_RDWR);
							close(slaveSocket);
							socketsToRemove.push_back(slaveSocket);
						}
						else if (recvSize != 0)
						{
							sendToEverybody(std::string(buff, recvSize), SlaveSockets);
							//send(slaveSocket, buff, 1024, MSG_NOSIGNAL);
						}
					}
				});

		std::for_each(socketsToRemove.begin(), socketsToRemove.end(), [&] (int removeSocket)
						{
							SlaveSockets.erase(removeSocket);
						});

		// process master socket
		if (FD_ISSET(masterSocket, &Set))
		{
			int slaveSocket = accept(masterSocket, 0 , 0);
			if (set_nonblock(slaveSocket) == -1)
						std::cout << "Failed to set slave socket as non-block" << std::endl;

			SlaveSockets.insert(slaveSocket);

			sendToEverybody("New guy joined\n", SlaveSockets);
		}
	}
}
