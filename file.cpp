#include <sys/socket.h>
#include <sys/epoll.h>
#include <cstdlib>  
#include <string>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <deque>
#include <sstream>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <fstream>
using namespace std;

int main()
{
	sockaddr_in server;
	try
	{
		int fd_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (fd_socket < 0)
			perror("socket() call failed");
		server.sin_family = AF_INET;
		
		int binf_fd = bind()

	}
	catch (...)
	{
		// cerr << e.what() << endl;
	}
}