/*
 * main.cpp
 *
 *  Created on: Jun 23, 2016
 *      Author: denis
 */

#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <iostream>

static const int BUFFER_SIZE = 1024;

int main()
{
	std::string inputFifo("/home/bakhvalo/in.fifo");

	remove(inputFifo.c_str());
	if (mkfifo(inputFifo.c_str(), 0666))
	{
		std::cout << "inputFifo was not created.\n";
		return 1;
	}
	int fdIn = open(inputFifo.c_str(), O_RDONLY | O_NONBLOCK);
	if (fdIn <= 0)
	{
		std::cout << "inputFifo descriptor was not created.\n";
		return 1;
	}

	std::string outputFifo("/home/bakhvalo/out.fifo");

	remove(outputFifo.c_str());
	if (mkfifo(outputFifo.c_str(), 0666))
	{
		std::cout << "outputFifo was not created.\n";
		return 1;
	}
	int fdOut = open(outputFifo.c_str(), O_WRONLY);
	if (fdOut <= 0)
	{
		std::cout << "outputFifo descriptor was not created.\n";
		return 1;
	}

	while (true)
	{
		char buf[BUFFER_SIZE];
		memset(buf, '\0', BUFFER_SIZE);
		int len = read(fdIn, buf, BUFFER_SIZE - 1);
		if (len > 0 )
		{
			write(fdOut, buf, len);
		}
	}
	return 0;
}

