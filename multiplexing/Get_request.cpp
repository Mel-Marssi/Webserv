#include "multiplexing.hpp"
#include "request.hpp"

void    Request::parse_url_prot(string meth)
{
    map<string, string>::iterator it;
    size_t i;

    this->Path = "";
    this->Protocole = "";
    this->file_get = "";
    it = header_request.find(meth);
    if(it->first == meth)
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
<<<<<<< HEAD
    // cout << this->Path  << endl;
        // cout << "---------||||||------\n";
=======
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
>>>>>>> 5d0219d (GET)
}


void Request::Generate_req_first(epoll_event &event, servers &config, int epoll_fd, map<string, string> &m)
{
    //Pars__Line__Get :
    this->parse_url_prot("GET");
    //Check__if__Loc__exisite_in__Server :
    int index = get_right_index(config.server, atoi(_port.c_str()), _host, config.get_server_name(atoi(_port.c_str())));

<<<<<<< HEAD
    // cout << Path << " " << full_Path << endl;
    if ((config[index].get_loc_path_location(this->Path).empty()) && (Path.find(".") == string::npos))
        error_page(event, epoll_fd, "404", config);
    //check__if__methode__alowed :
    else if ((config[index].get_loc_get(this->Path) == 0) && (Path.find(".") == string::npos))
        error_page(event, epoll_fd, "405", config);

    //if Path Empty__serve__the __server:
    else if (this->Path == "/")
=======
    this->full_Path.insert(0, root);
    if ((config.get_loc_get(this->Path) == 0) && (config.get_loc_get("GET") == 0) && (Path.find(".") == string::npos))
        error_page(event, epoll_fd, "405");
    if (config.get_loc_get(this->Path) && (config.get_loc_root(this->Path) != ""))
>>>>>>> 5d0219d (GET)
    {
        if (!config[index].get_index().empty())
        {
            dire = opendir(config[index].get_root().c_str());
            if (dire)
            {
                op.open((config[index].get_root() + config[index].get_index()).c_str());
                if (op.is_open())
                {
                    read_for_send(m);
                    if (op.eof())
                        end_of_file(event, epoll_fd);
                }//not_found or forbiden
                else
                    error_page(event, epoll_fd, "404", config);
            }
            else
                error_page(event, epoll_fd, "404", config);
        }
        else
        {
            //check_auto_index
            if (config[index].get_loc_auto_index(this->Path))
            {
                string str = config[index].get_root();
                if (full_Path[0] != '/' && str[str.length() - 1] != '/')
                   root_page(event, epoll_fd, ((str + this->full_Path)));
                else
                   root_page(event, epoll_fd, ((str.erase((str.length() - 1), 1) + this->full_Path)));
            }
            else//forbiden or not allowed
                error_page(event, epoll_fd, "404", config);
        }
    }
    else if (!(config[index].get_loc_path_location(this->Path).empty()) && !(config[index].get_loc_root(this->Path).empty()))
    {
        string root = config[index].get_loc_root(this->Path);
        if (root[root.length() - 1] == '/')// && (this->full_Path[0] == '/'))
            this->full_Path.erase(0,1);
        this->full_Path.insert(0, root);
        //check__redirection :
        if (config[index].get_loc_redirection(this->Path) == "")
        {
            string str = this->Path;
            dire = opendir((root + str.erase(0,1)).c_str());
<<<<<<< HEAD
            if (dire)// && (config[index].get_loc_index(this->Path).empty()))
            {
                if ((file_get == "") && (this->Path_bef[Path_bef.length() - 1] != '/'))
                    redirection_content_backSlash(event, epoll_fd, config);
                else if ((file_get == "") && (!(config[index].get_loc_index(this->Path).empty()))) // ===============
                {
                    file_get = config[index].get_loc_index(this->Path);
                    op.open((this->full_Path + config[index].get_loc_index(this->Path)).c_str());
                    if (op.is_open())
=======
            if (dire)
            {
                if ((file_get == "") && (this->Path_bef[Path_bef.length() - 1] != '/'))
                    redirection_content_backSlash(event, epoll_fd, config);
                // if (full_Path[full_Path.length() - 1] != '/')
                //     op.open((this->full_Path + "/" +this->file_get).c_str());
                // else
            cout << "respons=-------------------\t"<< (this->full_Path)<< endl;
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
>>>>>>> 5d0219d (GET)
                    {
                        read_for_send(m);
                        if (op.eof())
                            end_of_file(event, epoll_fd);
                    }
                    else //not_found_or_forbiden :
                        error_page(event, epoll_fd, "404", config);
                }//check auto index
                else if ((file_get != ""))
                {
                    op.open((this->full_Path).c_str());
                    if (op.is_open())
                    {
                        read_for_send(m);
                        if (op.eof())
                            end_of_file(event, epoll_fd);
                    }
                    else //not_found_or_forbiden :
                        error_page(event, epoll_fd, "404", config);
                }//if index not existe  
                else if (config[index].get_loc_auto_index(this->Path))
                {
                    close_dir();
                    root_page(event, epoll_fd, this->full_Path); 
                }
<<<<<<< HEAD
=======
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
>>>>>>> 5d0219d (GET)
            }
            else if (config[index].get_loc_auto_index(this->Path)) // hna ralat ==================
                root_page(event, epoll_fd, (config[index].get_loc_root(this->Path) + "/"+this->Path));
            else
                error_page(event, epoll_fd, "404", config);
        }
        else
            redirection_content(event, epoll_fd, config, index);
    }
    else if (this->Path.find(".") != string::npos)
    {
        string root = config[index].get_root();
        if (root[root.length() - 1] == '/')// && (this->full_Path[0] == '/'))
            this->full_Path.erase(0,1);
        this->full_Path.insert(0, root);
        op.open((this->full_Path).c_str());
        if (op.is_open())
        {
        // cout << "-->>> "<< full_Path << endl;
            read_for_send(m);
            if (op.eof())
            {
<<<<<<< HEAD
                cout << "sallllla\n";
                end_of_file(event, epoll_fd);
=======
                string str;
                if (Path[Path.length() - 1] == '/')
                    str = Path + file_get;
                else
                    str = Path + "/" + file_get;
                op.open(str.c_str());
                cout << Path << " = " << file_get << endl;
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
>>>>>>> 5d0219d (GET)
            }
        }
        else //not_found_or_forbiden :
            error_page(event, epoll_fd, "404", config);

    }
    // else if ((config.get_loc_get(this->Path) && (config.get_loc_auto_index(this->Path) == 1)))
    // {
    //     int serv = get_right_index(config.server, (int)atoi(Port.c_str()), Host, server_name);
    //     if (serv >= 0)
    //     {
    //         this->Path = config.get_root(config[serv].get_port());
    //         this->file_get = config.get_index(config[serv].get_port());

    //         dire = opendir(Path.c_str());
    //         if (dire)
    //         {
    //             string str;
    //             if (Path[Path.length() - 1] == '/')
    //                 str = Path + file_get;
    //             else
    //                 str = Path + "/" + file_get;
    //             op.open(str.c_str());
    //             // cout << Path << " = " << file_get << endl;
    //             if (op.is_open() && file_get != "")
    //             {
    //                 check_req = 1;
    //                 string head = read_buff(m);
    //                 len = head.length();

    //                 send(event.data.fd, head.c_str(), len, 0);
    //                 line = "";
    //                 if (op.eof())
    //                 {
    //                     fin_or_still = finish;
    //                     epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
    //                     close(event.data.fd);
    //                     op.close();
    //                     if (dire)
    //                         {closedir(dire), dire = NULL;}
    //                 }
    //             }
    //             else
    //             {
    //                 closedir(dire);
    //                 dire = NULL;
    //                 root_page(event, epoll_fd, this->Path);
    //             }
    //         }
    //         else
    //         {
    //             error_page(event, epoll_fd, "404", config);
    //             fin_or_still = finish;
    //             epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
    //             close(event.data.fd);
    //         }
    //     }
    //     else
    //     {
    //         if (dire)
    //             {closedir(dire), dire = NULL;}
    //     }
    // }
    // else if (this->Path.find(".") != string::npos)
    // {
    //     int serv = get_right_index(config.server, (int)atoi(Port.c_str()), Host, server_name);
    //     if (serv >= 0)
    //     {
    //         this->full_Path = config.get_root(config[serv].get_port());
    //         dire = opendir(full_Path.c_str());
    //         if (dire)
    //         {
    //             op.open((this->full_Path + (Path)).c_str());
    //             if (op.is_open())
    //             {
    //             // cout << "----------\t"<< (this->full_Path + (Path.erase(0, 1))) << endl; 
    //                 check_req = 1;
    //                 string head = read_buff(m);
    //                 // cout << head << endl;
    //                 // cout << "===============\n";
    //                 if (con_type.find("text") != string::npos)
    //                 {
    //                     len = strlen(head.c_str());
    //                     send(event.data.fd, head.c_str(), len, 0);            
    //                 }
    //                 else
    //                 {
    //                     len = head.length();
    //                     send(event.data.fd, head.c_str(), len, 0);
    //                 }

    //                 // len = head.length();
    //                 // check_req = 1;
    //                 // send(event.data.fd, head.c_str(), len, 0);
    //                 line = "";
    //                 if (op.eof())
    //                 {
    //                     fin_or_still = finish;
    //                     epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
    //                     close(event.data.fd);
    //                     op.close();
    //                     if (dire)
    //                         {closedir(dire), dire = NULL;}
    //                 }
    //                 //hna 
    //             }
    //             else
    //             {
    //                 closedir(dire);
    //                 dire = NULL;
    //                 root_page(event, epoll_fd, this->Path);
    //             }
    //         }
    //     }
    //     // cout << this->full_Path <<"\t--\t"<< this->Path <<"\t--\t" << root << endl;
    //     // dire = opendir()
    //     error_page(event, epoll_fd, "404", config);
    // }
    else
    {
<<<<<<< HEAD

        error_page(event, epoll_fd, "404", config);
=======
        // cout << this->full_Path <<"\t--\t"<< this->Path <<"\t--\t" << root << endl;
        // dire = opendir()
        int serv = get_right_index(config.server, (int)atoi(Port.c_str()), Host, server_name);
        if (serv >= 0)
        {
            this->Path = config.get_root(config[serv].get_port());
            this->file_get = config.get_index(config[serv].get_port());

            dire = opendir(Path.c_str());
        error_page(event, epoll_fd, "404");
>>>>>>> 5d0219d (GET)
    }
    (void)m;
}

void Request::Generate_req_second(epoll_event &event, int epoll_fd)
{
    if (op.is_open())
    {
        // cout << "----------------------\n";
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

void Request::default_error(string key, int fd)
{
        string head;
        string status = get_status_code(key);
        // int size = (status.length() * 3) + (key.length() * 2) + 119;
        int size = 19 + status.length() + 25 + key.length() + 1 + status.length() + 19;
        std::ostringstream oss;
        oss << size;
        head  = "HTTP/1.1 ";
        head += key;
        head += status;
        head += "\r\n";
        head += "Content-Type: text/html\r\nContent-Lenght:";
        head += oss.str();
        head += "\r\n\r\n";
        head += "<html><head><title>" + status +"</title></head><body><h1>" + key + " " + status +"</h1></body></html>";

        len = head.length();
        line = "";
        this->fin_or_still = finish;
        send(fd, head.c_str(), len, 0);
}

void Request::error_page(epoll_event &event, int epoll_fd, string key, servers &config)
{
    int index = get_right_index(config.server, atoi(_port.c_str()), _host, config.get_server_name(atoi(_port.c_str())));

    close_dir();
    string str = config[index]._error_book[atoi(key.c_str())];

    std::ifstream ovp(str.c_str());
    if ((ovp.is_open() && fin_or_still != finish))
    {
        string head;
        getline(ovp, line, '\0');
        size_t len = line.length();
        std::ostringstream oss;
        oss << len;
        head += "HTTP/1.1 ";
        head += key;
        head += get_status_code(key);
        head += "\r\n";
        head += "Content-Type: text/html\r\nContent-Lenght:";
        head += oss.str();
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
    else
    {
        default_error(key, event.data.fd);
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
        close(event.data.fd);
    }
}