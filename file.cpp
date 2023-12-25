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
#include <sys/stat.h>
using namespace std;

void Request::Generate_req_first(epoll_event &event, servers &config, int epoll_fd, map<string, string> &m)
{
    check_req = 1;
    this->parse_url_prot();
    string root = config.get_loc_root(this->Path);
    if (root[root.length() - 1] == '/')
        this->full_Path.erase(0,1);

    this->full_Path.insert(0, root);
    if (config.get_loc_get(this->Path))
    {
        if (config.get_loc_redirection(this->Path) == "")
        {
            string str = this->Path;
            // dire = opendir((str.erase(0,1)).c_str());
            // cout << full_Path << endl;
            dire = opendir(this->full_Path);
            if (dire)
            {
                struct dirent* entre;
                
                while ((entre = readdir(dire)) != NULL)
                {   
                    if ((config.get_loc_index(this->Path) == entre->d_name) && (config.get_loc_index(this->Path) == this->file_get))
                    {                   
                        op.open(this->full_Path.c_str());
                        if (op.is_open())
                        {
                            // cout << "====================\n";
                            entre_or_not = 1;
                            string head;
                            // //to get the size =======
                            // op.seekg(0, ios::end);
                            // streampos fileSize = op.tellg();
                            // op.seekg(0, ios::beg);
                            struct stat fileStat;
                            stat(this->full_Path.c_str(), &fileStat);
                            std::stringstream fileSize;
                            fileSize << fileStat.st_size;
                            // //=======================
                            
                            char buffer[1024];
                            op.read(buffer, 1024);
                            line.append(buffer, 1024);
                            head += "HTTP/1.1 200 ok\r\nContent-Type: ";
                            con_type = get_content_type(m);
                            head += con_type;
                            head += "\r\nContent-Lenght:";
                            size_t len;// = (size_t)filStat.st_size;
                            head += fileSize.str();
                            head += "\r\n\r\n";
                            head += line;
                            len = head.length();

                            send(event.data.fd, head.c_str(), len, 0);
                            line = "";
                            if (op.eof())
                            {
                                fin_or_still = finish;
                                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
                                close(event.data.fd);
                                op.close();
                                closedir(dire);
                                dire = NULL;
                            }
                            break;
                        }
                        else
                        {
                            closedir(dire);
                            dire = NULL;
                        }
                    }
                }
                if (entre_or_not == 0 && config.get_loc_auto_index(this->Path) == 1)
                {
                    root_page(event, epoll_fd, this->full_Path);             
                }
            }
        }
        else
            redirection_content(event, epoll_fd, config);
    }
    else
        error_page(event, epoll_fd);
    (void)m;
}