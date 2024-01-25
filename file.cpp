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
	DIR *dire;

	dire = opendir("./test9");
	if (dire)
		cout << "dazeed" << endl;
}