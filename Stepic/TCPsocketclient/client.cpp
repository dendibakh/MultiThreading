/*
 * server.cpp
 *
 *  Created on: Jun 5, 2016
 *      Author: denis
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

int main(int argc, char** argv)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1)
		std::cout << "Failed to open socket" << std::endl;

	// try UNIX sockets
	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(12345);
	sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1

	if (connect(sock, (sockaddr*)&sa, sizeof(sa)) == -1)
		std::cout << "Failed to connect the socket" << std::endl;

	std::string buff("PING");
	if (argc > 1)
		buff = argv[1];

	if (send(sock, buff.data(), buff.size(), MSG_NOSIGNAL) == -1)
		std::cout << "Failed to send the message" << std::endl;

	if (recv(sock, (char*)buff.data(), buff.size(), MSG_NOSIGNAL) == -1)
		std::cout << "Failed to receive the message" << std::endl;

	shutdown(sock, SHUT_RDWR);

	std::cout << "received from server:" << std::endl << buff << std::endl;
}

