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
    i = 1;
    i = this->Path.find("/", i);
    if (i != string::npos)
    {
        this->file_get.insert(0, this->Path, i + 1, this->Path.length());
        this->Path_bef = this->Path;
        this->Path.erase(i, this->Path.length());
        this->full_Path = this->Path_bef;
    }
    else
        this->full_Path = this->Path;
    //=============Morad=====================
    it = header_request.find("Host");
    if (it != header_request.end())
    {
        size_t b = it->second.find(":");
        this->Host.insert(0, it->second, 0, b);
        if (Host == "127.0.0.1" || Host == "0.0.0.0")
        {
            Host = "127.0.0.1";
            server_name = "localhost";
        }
        else if (Host == "localhost")
        {
            server_name = Host;
            Host = "127.0.0.1";
        }
        else
        {
            server_name = Host;
        }
        size_t n = b;
        n = it->second.find(" ", n);
        this->Port.insert(0, it->second, b + 1, n);
        this->Port.erase(Port.length() - 1, Port.length());
        // cout <<it->second.length() << "|" << Port << "|"<< "\n";
    }
}

std::string Request::read_buff(map<string, string> &m)
{
    string head;
    stringstream size;
    // //to get the size =======
    op.seekg(0, ios::end);
    streampos fileS = op.tellg();
    op.seekg(0, ios::beg);
    // struct stat fileStat;
    // stat(this->Path.c_str(), &fileStat);
    // std::stringstream fileSize;
    // fileSize << fileStat.st_size;
    // //=======================

    char buffer[1024];
    memset(buffer, 0, 1024 );
    op.read(buffer, 1024);
    line.append(buffer, 1024);
    head += "HTTP/1.1 200 ok\r\nContent-Type: ";
    con_type = get_content_type(m);
    head += con_type;
    head += "\r\nContent-Lenght:";
    size << fileS;
    head += size.str();
    // size_f = size;
    // size_f -= 1024;
    // cout << fileSize.str() << "******\n";
    head += "\r\n\r\n";
    head += line;
    line = "";
    return (head);
}

void Request::Generate_req_first(epoll_event &event, servers &config, int epoll_fd, map<string, string> &m)
{
    this->parse_url_prot();
    string root = config.get_loc_root(this->Path);
    if (root[root.length() - 1] == '/')
        this->full_Path.erase(0,1);

    this->full_Path.insert(0, root);
    if ((config.get_loc_get(this->Path) == 0) && (config.get_loc_get("GET") == 0) && (Path.find(".") == string::npos))
        error_page(event, epoll_fd, "405");
    // cout << "respons=-------------------\t"<< (this->Path)<< endl;
    if (config.get_loc_get(this->Path) && (config.get_loc_root(this->Path) != ""))
    {
                // cout << "==============\n";
        if (config.get_loc_redirection(this->Path) == "")
        {
            string str = this->Path;
            dire = opendir((root + str.erase(0,1)).c_str());
            if (dire)
            {
                if ((file_get == "") && (this->Path_bef[Path_bef.length() - 1] != '/'))
                    redirection_content_backSlash(event, epoll_fd, config);
                // if (full_Path[full_Path.length() - 1] != '/')
                //     op.open((this->full_Path + "/" +this->file_get).c_str());
                // else
                op.open((this->full_Path ).c_str()); 
                if (op.is_open() && this->file_get != "")
                {
                    entre_or_not = 1;
                    check_req = 1;
                    string head = read_buff(m);
                    len = head.length();

                    send(event.data.fd, head.c_str(), len, 0);
                    line = "";
                    if (op.eof())
                    {

                        fin_or_still = finish;
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
                        close(event.data.fd);
                        op.close();
                        if (dire)
                            {closedir(dire), dire = NULL;}
                    }
                }
                else if (config.get_loc_auto_index(this->Path) == 1)
                {
                    if (dire)
                        {closedir(dire), dire = NULL;}
                    root_page(event, epoll_fd, this->full_Path); 
                }
                else
                {
                    if (dire)
                        {closedir(dire), dire = NULL;}
                    error_page(event, epoll_fd, "404");
                }
            }
            else
            {
                cout << "========================\n";   
                if (dire)
                    {closedir(dire), dire = NULL;}
                error_page(event, epoll_fd, "404");
            }
        }
        else
            redirection_content(event, epoll_fd, config);
    }
    else if ((config.get_loc_get(this->Path) && (config.get_loc_auto_index(this->Path) == 1)))
    {
        int serv = get_right_index(config.server, (int)atoi(Port.c_str()), Host, server_name);
        if (serv >= 0)
        {
            this->Path = config.get_root(config[serv].get_port());
            this->file_get = config.get_index(config[serv].get_port());

            dire = opendir(Path.c_str());
            if (dire)
            {
                string str;
                if (Path[Path.length() - 1] == '/')
                    str = Path + file_get;
                else
                    str = Path + "/" + file_get;
                op.open(str.c_str());
                // cout << Path << " = " << file_get << endl;
                if (op.is_open() && file_get != "")
                {
                    check_req = 1;
                    string head = read_buff(m);
                    len = head.length();

                    send(event.data.fd, head.c_str(), len, 0);
                    line = "";
                    if (op.eof())
                    {
                        fin_or_still = finish;
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
                        close(event.data.fd);
                        op.close();
                        if (dire)
                            {closedir(dire), dire = NULL;}
                    }
                }
                else
                {
                    closedir(dire);
                    dire = NULL;
                    root_page(event, epoll_fd, this->Path);
                }
            }
            else
            {
                error_page(event, epoll_fd, "404");
                fin_or_still = finish;
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
                close(event.data.fd);
            }
        }
        else
        {
            if (dire)
                {closedir(dire), dire = NULL;}
        }
    }
    else if (this->Path.find(".") != string::npos)
    {
        int serv = get_right_index(config.server, (int)atoi(Port.c_str()), Host, server_name);
        if (serv >= 0)
        {
            this->full_Path = config.get_root(config[serv].get_port());
            dire = opendir(full_Path.c_str());
            if (dire)
            {
                // string str;
                // if (Path[Path.length() - 1] == '/')
                //     str = Path + file_get;
                // else
                //     str = Path + "/" + file_get;
                // cout << Path << " = " << file_get << endl;
                op.open((this->full_Path + (Path)).c_str());
                if (op.is_open())
                {
                // cout << "----------\t"<< (this->full_Path + (Path.erase(0, 1))) << endl; 
                    check_req = 1;
                    string head = read_buff(m);
                    // cout << head << endl;
                    cout << "===============\n";
                    if (con_type.find("text") != string::npos)
                    {
                        len = strlen(head.c_str());
                        send(event.data.fd, head.c_str(), len, 0);            
                    }
                    else
                    {
                        len = head.length();
                        send(event.data.fd, head.c_str(), len, 0);
                    }

                    // len = head.length();
                    // check_req = 1;
                    // send(event.data.fd, head.c_str(), len, 0);
                    line = "";
                    if (op.eof())
                    {
                        fin_or_still = finish;
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
                        close(event.data.fd);
                        op.close();
                        if (dire)
                            {closedir(dire), dire = NULL;}
                    }
                    //hna 
                }
                else
                {
                    closedir(dire);
                    dire = NULL;
                    root_page(event, epoll_fd, this->Path);
                }
            }
        }
        // cout << this->full_Path <<"\t--\t"<< this->Path <<"\t--\t" << root << endl;
        // dire = opendir()
        error_page(event, epoll_fd, "404");
    }
    else
    {
            cout << "===========\t" << full_Path << "\t" << Path<< endl;
        error_page(event, epoll_fd, "404");
    }
    (void)m;
}

void Request::Generate_req_second(epoll_event &event, int epoll_fd)
{
    if (op.is_open())
    {
        cout << "----------------------\n";
        char buffer[1024];
        memset(buffer, 0, 1024 );
        op.read(buffer, sizeof(buffer));
        line.append(buffer, 1024);

        size_t len;
        if (con_type.find("text") != string::npos)
        {
            len = strlen(line.c_str());
            send(event.data.fd, line.c_str(), len, 0);            
        }
        else
        {
            len = line.length();
            send(event.data.fd, line.c_str(), len, 0);
        }
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
            if (dire)
                {closedir(dire), dire = NULL;}
        }
    }
}

void Request::error_page(epoll_event &event, int epoll_fd, string key)
{
    string str = "./prblm/"+key+".html";
    // string str = m;
    std::ifstream ovp(str.c_str());
    if (ovp.is_open() && fin_or_still != finish)
    {
        string head;
        getline(ovp, line, '\0');
        size_t len = line.length();
        head += "HTTP/1.1 ";
        head += key;
        head += get_status_code(key);
        head += "\r\n";
        head += "Content-Type: text/html\r\nContent-Lenght:";
        head += len;
        head += "\r\n\r\n";
        head += line;
        len = head.length();
        line = "";
        this->fin_or_still = finish;
        send(event.data.fd, head.c_str(), len, 0);
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
        close(event.data.fd);
        ovp.close();
        if (dire != NULL)
        {
            closedir(dire);
            dire = NULL;
        }
    }
    // else
    // {
    //     //mouad
    // }
}