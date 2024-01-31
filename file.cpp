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
	ifstream file("./fich.pdf");
	if (file.is_open())
	{
		cout << "open" << endl;
	}
	else
		cout << "not open" << endl;
}