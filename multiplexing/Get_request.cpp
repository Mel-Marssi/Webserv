#include "multiplexing.hpp"
#include "request.hpp"

void    Request::parse_url_prot()
{
    map<string, string>::iterator it;
    size_t i;

    this->Path = "";
    this->Protocole = "";
    this->file_get = "";
    it = header_request.find("GET");
    if(it->first == "GET")
    {
        i = it->second.find(" ");
        this->Path.insert(0, it->second, 0, i);
        i++;
        this->Protocole.insert(0, it->second, i, it->second.length());
    }
    this->full_Path = this->Path;
    i = 1;
    i = this->Path.find("/", i);
    if (i != string::npos)
    {
        this->file_get.insert(0, this->Path, i + 1, this->Path.length());
        this->Path.erase(i, this->Path.length());
    }
    cout << this->Path << "\t" << endl;
}

void Request::Generate_req_first(epoll_event &event, servers &config, int epoll_fd, map<string, string> &m)
{
    check_req = 1;
    this->parse_url_prot();
    // cout << config.get_loc_root(Path) << endl;
    // if (config.get_loc_root(Path) != "")
    //     root_page(event, epoll_fd, config);
    this->full_Path.erase(0,1);
    this->full_Path.insert(0, config.get_loc_root(this->Path));
    if (config.get_loc_get(this->Path))
    {
        if (config.get_loc_redirection(this->Path) == "")
        {
            string str = this->Path;
            dire = opendir((str.erase(0,1)).c_str());
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
                    // cout << "==============\n";
                    root_page(event, epoll_fd, config);
        // error_page(event, epoll_fd);
                
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

void Request::Generate_req_second(epoll_event &event, int epoll_fd)
{
    if (op.is_open())
    {
        char buffer[1024];
        op.read(buffer, sizeof(buffer));
        line.append(buffer, 1024);

        size_t len;
        len = line.length();
        send(event.data.fd, line.c_str(), len, 0);
        line = "";
        
        if (op.eof())
        {
            len = line.length();
            send(event.data.fd, line.c_str(), len, 0);
            line = "";
            fin_or_still = finish;
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
            close(event.data.fd);
            op.close();
            closedir(dire);
            dire = NULL;
        }
    }
}

void Request::error_page(epoll_event &event, int epoll_fd)
{
    std::ifstream op("./prblm/file_not_found.html");
    if (op.is_open() && fin_or_still != finish)
    {
        string head;
        getline(op, line, '\0');
        size_t len = line.length();
        head += "HTTP/1.1 404 Not Found\r\nContent-Type:  text/html\r\nContent-Lenght:";
        head += len;
        head += "\r\n\r\n";
        head += line;
        len = head.length();
        line = "";
        this->fin_or_still = finish;
        send(event.data.fd, head.c_str(), len, 0);
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
        close(event.data.fd);
        if (dire != NULL)
        {
            closedir(dire);
            dire = NULL;
        }
    }
}