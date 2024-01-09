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

int main()
{
	std::ifstream la("/tmp/_1704622471");
	if (la.is_open())
		std::cout << "------------\n";
	else
		std::cout << "++++++++++++++\n";
}