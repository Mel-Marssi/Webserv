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
        if (this->file_get.find("?") != string::npos)
        {
            size_t o;
            
            o = this->file_get.find("?");
            this->Query_String.insert(0, file_get, o + 1, file_get.length());
            full_Path[full_Path.find("?")] = '\0';
            // full_Path.erase(o, full_Path.length());
            file_get.erase(o, file_get.length());
        }
        else
            Query_String = "";
    }
    else
        this->full_Path = this->Path;
}


void Request::Generate_req_first(epoll_event &event, servers &config, int epoll_fd, map<string, string> &m)
{
    //Pars__Line__Get :
    this->parse_url_prot("GET");
    
    //Check__if__Loc__exisite_in__Server :
    int index = get_right_index(config.server, atoi(_port.c_str()), _host, config.get_server_name(atoi(_port.c_str())));

    if ((config[index].get_loc_path_location(this->Path).empty()) && (Path.find(".") == string::npos))
        error_page(event, epoll_fd, "404", config);
    //check__if__methode__alowed :
    else if ((config[index].get_loc_get(this->Path) == 0) && (Path.find(".") == string::npos))
        error_page(event, epoll_fd, "405", config);

    //if Path Empty__serve__the __server:
    else if (this->Path == "/")
    {
        if (!config[index].get_index().empty())
        {
            dire = opendir(config[index].get_root().c_str());
            if (dire)
            {
                op.open((config[index].get_root() + config[index].get_index()).c_str());
                if (op.is_open())
                {
                    read_for_send(m, 0);
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
            else//forbiden 
            {
                error_page(event, epoll_fd, "403", config);
            }
        }
    }
    else if (!(config[index].get_loc_path_location(this->Path).empty()))
    {
        string root;
        if (!config[index].get_loc_root(this->Path).empty())
            root = config[index].get_loc_root(this->Path);
        else
            root = config[index].get_root();
        if (root[root.length() - 1] == '/')// && (this->full_Path[0] == '/'))
            this->full_Path.erase(0,1);
        this->full_Path.insert(0, root);
        //check__redirection :
        if (config[index].get_loc_redirection(this->Path) == "")
        {
            string str = this->Path;
            dire = opendir((root + str.erase(0,1)).c_str());
            if (dire)
            {
                if ((file_get == "") && (this->Path_bef[Path_bef.length() - 1] != '/'))
                    redirection_content_backSlash(event, epoll_fd, 0);
                else if ((file_get == "") && (!(config[index].get_loc_index(this->Path).empty()))) // ===============
                {
                    if (this->Path.find("cgi") != string::npos)
                    {
                        php_cgi(*this, config[index]);
                        op.open(cgi_file.c_str());
                        if (op.is_open())
                        {
                            read_for_send(m, 1);
                            if (op.eof())
                                end_of_file(event, epoll_fd);
                        }
                    }
                    op.open((this->full_Path + config[index].get_loc_index(this->Path)).c_str());
                    if (op.is_open())
                    {
                        read_for_send(m, 0);
                        if (op.eof())
                            end_of_file(event, epoll_fd);
                    }
                    else //not_found_or_forbiden :
                        error_page(event, epoll_fd, "404", config);
                }//check auto index
                else if ((file_get != ""))
                {
                    if (this->Path.find("cgi") != string::npos)
                    {
                        php_cgi(*this, config[index]);
                        op.open(cgi_file.c_str());
                        if (op.is_open())
                        {
                            read_for_send(m, 1);
                            if (op.eof())
                                end_of_file(event, epoll_fd);
                        }
                    }
                    else
                    {
                        op.open((this->full_Path).c_str());
                        if (op.is_open())
                        {
                            read_for_send(m, 0);
                            if (op.eof())
                                end_of_file(event, epoll_fd);
                        }
                        else //not_found_or_forbiden :
                            error_page(event, epoll_fd, "404", config);
                    }
                }//if index not existe  
                else if (config[index].get_loc_auto_index(this->Path))
                    root_page(event, epoll_fd, this->full_Path); 
            }
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
        if (this->Path.find("cgi") != string::npos)
        {
            php_cgi(*this, config[index]);
        }
        else
        {
            op.open((this->full_Path).c_str());
            if (op.is_open())
            {
                read_for_send(m, 0);
                if (op.eof())
                    end_of_file(event, epoll_fd);
            }
            else //not_found_or_forbiden :
                error_page(event, epoll_fd, "404", config);
        }
    }
    else
        error_page(event, epoll_fd, "404", config);
    (void)m;
}
void Request::Generate_req_second(epoll_event &event, int epoll_fd)
{
    if (op.is_open())
    {
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
            end_of_file(event, epoll_fd);
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
        send(event.data.fd, head.c_str(), len, 0);
        end_of_file(event, epoll_fd);
    }
    else
    {
        default_error(key, event.data.fd);
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
        close(event.data.fd);
    }
}