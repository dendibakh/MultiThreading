/*
 * client.cpp
 *
 *  Created on: Jun 5, 2016
 *      Author: denis
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

// This function is portable for setting socket in non-blocking mode
#include <sys/ioctl.h>

int set_nonblock(int fd)
{
	int flags;
	#if defined(O_NONBLOCK)
		if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
			﻿flags = 0;
		﻿return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	#else
		flags = 1;
		return ioctl(fd, FIONBIO, &flags);
	#endif
}

int main()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1)
		std::cout << "Failed to open socket" << std::endl;

	// try UNIX sockets also
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(12345);
	sa.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0

	if (bind(sock, (sockaddr*)&sa, sizeof(sa)) == -1)
			std::cout << "Failed to bind socket" << std::endl;

	if (listen(sock, SOMAXCONN) == -1)
			std::cout << "Failed to listen to socket" << std::endl;

	while (true)
	{
		int slaveSock = accept(sock, 0, 0);
		char buff[5] = {0,0,0,0,0};
		recv(slaveSock, buff, 4, MSG_NOSIGNAL);
		send(slaveSock, buff, 4, MSG_NOSIGNAL);
		shutdown(slaveSock, SHUT_RDWR);
		close(slaveSock);
		std::cout << buff << std::endl;
	}

}
