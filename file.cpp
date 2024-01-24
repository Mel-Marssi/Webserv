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
#include <fstream>
using namespace std;

int main()
{
	char buffer[1024];
	char *b = realpath("./test/vidiiii.mp4 ", buffer);
	if (b == NULL)
		cout << "NULL" << endl;
	else
		cout << "buffer" << endl;
}