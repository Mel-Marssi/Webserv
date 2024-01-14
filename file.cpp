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
// char *realpath(const char *restrict path,char *restrict resolved_path);

	char *u = "./";
	char *b= realpath("./../bin", u);
	cout << b << endl;
}