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

// int check_permission(std::string str)
// {
//     if (access((str).c_str(), F_OK) != -1)
//     {
//         if ((access((str).c_str(), R_OK) && access((str).c_str(), W_OK) && access((str).c_str(), X_OK)) == 0)
//             return (1);
//         else
//             return (0);
//     }
//     return (0);
// }

int main() {
    DIR* dire;

    dire = opendir("./fff");
    if (dire)
    {
        struct dirent* entre ;
        (entre = readdir(dire));
        if (entre == NULL)
            std::remove("./fff");
        else
            std::cout << "00000\n";
    }
    else
        std::cout << "Madkhelxx\n";
}
