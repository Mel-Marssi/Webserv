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
#include <cstdio>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/time.h>
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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
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
		server.sin_addr.s_addr = inet_addr("127.0.0.1");
		server.sin_port = htons("8080");
		int binf_fd = bind(fd_socket, (const sockaddr *)&server, sizeof(server));
		if (binf_fd < 0)
			perror("bind() call failed");
		int listen_fd = listen(fd_socket, 0);
		if (listen_fd < 0)
			perror("listen() call failed");
		int accept_fd = accept(fd_socket, NULL, NULL);
		if (accept_fd < 0)
			perror("accept() call failed");
	}
	catch (...)
	{
		// cerr << e.what() << endl;
	}
}