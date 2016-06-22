/*
 * main.cpp
 *
 *  Created on: Jun 22, 2016
 *      Author: bakhvalo
 */

#include <stdio.h>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>

#include <string>
#include <list>
#include <algorithm>
#include <vector>
#include <iostream>

struct Command
{
	std::string cmd;
	std::list<std::string> args;
};

Command parseOneCommand(const std::string& command)
{
	Command retCmd;
	std::string::const_iterator it = command.begin();
	std::string::const_iterator lastPos = command.begin();
	while (it != command.end())
	{
		it = std::find(it, command.end(), ' ');

		size_t begin = lastPos - command.begin();
		size_t end = it - command.begin();
		if (retCmd.cmd.empty())
		{
			retCmd.cmd = command.substr(begin, end - begin);
		}
		else
		{
			retCmd.args.push_back(command.substr(begin, end - begin));
		}

		if (it != command.end())
			++it;
		lastPos = it;
	}
	return retCmd;
}

std::list<Command> parseInput(const std::string& input)
{
	std::list<Command> cmdList;
	std::string::const_iterator it = input.begin();
	std::string::const_iterator lastPos = input.begin();
	while (it != input.end())
	{
		it = std::find(it, input.end(), '|');

		size_t begin = lastPos - input.begin();
		size_t end = it - input.begin();
		cmdList.push_back(parseOneCommand(input.substr(begin, end - begin)));

		if (it != input.end())
			++it;
		lastPos = it;
	}
	return cmdList;
}

void executeCommand(const Command& cmd)
{
	std::vector<char*> args;
	args.push_back((char*)cmd.cmd.c_str());
	for (std::list<std::string>::const_iterator iter = cmd.args.begin(); iter != cmd.args.end(); ++iter)
	{
		args.push_back((char*)iter->c_str());
	}
	args.push_back((char*)NULL);
	execvp(args[0], args.data());
}

std::string removeSpecialSymbols(const std::string& str)
{
	std::string retStr(str);
	retStr.erase( std::remove(retStr.begin(), retStr.end(), '\r'), retStr.end() );
	retStr.erase( std::remove(retStr.begin(), retStr.end(), '\n'), retStr.end() );
	retStr.erase( std::remove(retStr.begin(), retStr.end(), '\t'), retStr.end() );
	return retStr;
}

int main(int argc, char** argv)
{
	std::list<Command> cmdList;

	if (argc > 1)
	{
		cmdList = parseInput(removeSpecialSymbols(argv[1]));
	}
	else
	{
		char buffer[128];
		if (fgets(buffer, 128, stdin) != NULL)
			cmdList = parseInput(removeSpecialSymbols(buffer));
	}

	for (std::list<Command>::iterator i = cmdList.begin(); i != cmdList.end(); ++i)
	{
		std::cout << i->cmd.c_str();
		if (!i->args.empty())
			std::cout << i->args.begin()->c_str();
		std::cout << '\n';
	}

	for (std::list<Command>::iterator i = cmdList.begin(); i != cmdList.end(); ++i)
	{
		int pfd[2];
		pipe(pfd);
		int childPid = fork();
		if (childPid)
		{
			// parent process
			std::list<Command>::iterator temp = i;
			if (++temp != cmdList.end())
			{
				close (STDOUT_FILENO);
				dup2(pfd[1], STDOUT_FILENO);
				close (pfd[1]);
				close (pfd[0]);
			}
			else
			{
				int out = open("/home/denis/result.out", O_RDWR|O_CREAT|O_TRUNC, 0600);
				if (out == -1)
					std::cout << "Was not able to open /home/denis/result.out\n";
				close (STDOUT_FILENO);
				dup2(out, STDOUT_FILENO);
			}
			executeCommand(*i);
			int status = 0;
			waitpid(childPid, &status, 0);
		}
		else
		{
			// child process
			close (STDIN_FILENO);
			dup2(pfd[0], STDIN_FILENO);
			close (pfd[1]);
			close (pfd[0]);
			// go to next command
		}
	}

	return 0;
}


