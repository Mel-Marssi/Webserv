#include "multiplexing.hpp"
#include "request.hpp"

void Request::parse_url_prot(string meth, int epoll_fd, epoll_event &event, servers &config)
{
    map<string, string>::iterator it;
    size_t i;

    //=====================================
    int index = get_right_index(config.server, atoi(_port.c_str()), _host, config.get_server_name(atoi(_port.c_str())));
    string root = config[index].get_root();
    //=====================================

    this->Path = "";
    this->Protocole = "";
    this->file_get = "";
    it = header_request.find(meth);
    if (it->first == meth)
    {
        i = it->second.find("HTTP");
        this->Path.insert(0, it->second, 0, i-1);
        // i++;
        this->Protocole.insert(0, it->second, i, it->second.length());
    }
    if (root[root.length() - 1] =='/' && Path[0] == '/')
        root.erase(root.length() - 1, root.length());
    // cout << Path << " ----==" <<endl;
    check_url_encoding();
        // cout << Path << "  =====--"<< endl;
    if (meth == "DELETE")
    {
        char buff[1024];
        char *b = realpath((root + Path).c_str(), buff);
        if (b == NULL)
            error_page(event, epoll_fd, "404", config);
        else
        {
            string tmp = b;
            string tmp2 = config[index].get_root();
            b = realpath(tmp2.c_str(), buff);
            tmp2 = b;
            if (tmp.find(tmp2) == string::npos)
                error_page(event, epoll_fd, "403", config);
        }
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
    if (check_body_get(event) == 1)
        return ;
    this->parse_url_prot("GET", epoll_fd, event, config);

    index_serv = get_right_index(config.server, atoi(_port.c_str()), _host, config.get_server_name(atoi(_port.c_str())));

    string root;
    if (!config[index_serv].get_loc_root(this->Path).empty())
        root = config[index_serv].get_loc_root(this->Path);
    else
        root = config[index_serv].get_root();

    if ((config[index_serv].get_loc_path_location(this->Path).empty()) && ((is_open_diir("." + Path) == 1)))
       status_pro = "404";
    else if ((config[index_serv].get_loc_get(this->Path) == 0) && ((is_open_diir("." + Path) == 1)))
        status_pro = "405";
    else if (this->Path == "/")
    {
        if (!config[index_serv].get_index().empty())
        {
            dire = opendir(config[index_serv].get_root().c_str());
            if (dire)
            {
                op.open((config[index_serv].get_root() + config[index_serv].get_index()).c_str());
                if (op.is_open())
                    read_for_send(event, m, 0);
                else
                    status_pro = "404";
            }
            else
                status_pro = "404";
        }
        else
        {
            if (config[index_serv].get_loc_auto_index(this->Path))
            {
                if (Path[0] != '/' && root[root.length() - 1] != '/')
                    root_page(event, ((root + Path)));
                else
                    root_page(event, ((root.erase((root.length() - 1), 1) + Path)));
            }
            else
                status_pro = "403";
        }
    }
    else if (!(config[index_serv].get_loc_path_location(this->Path).empty()))
    {
        if ((root[root.length() - 1] == '/') && (this->full_Path[0] == '/'))
            this->full_Path.erase(0, 1);
        this->full_Path.insert(0, root);
        // check__redirection :
        if (config[index_serv].get_loc_redirection(this->Path) == "")
        {
            string str = this->Path;
            close_dir();
            dire = opendir((root + str.erase(0, 1)).c_str());
            if (dire)
            {
                if ((file_get == "") && (this->Path_bef[Path_bef.length() - 1] != '/'))
                    redirection_content_backSlash(event, epoll_fd, 0);
                else if ((file_get == "") && (!(config[index_serv].get_loc_index(this->Path).empty()))) // ===============
                {
                    // update changes
                    if (this->Path.find("cgi") != string::npos)
                        find_cgi(config, index_serv); // ila f location dyal cgi kan index file -------------------------
                    else
                    {
                        op.open((this->full_Path + config[index_serv].get_loc_index(this->Path)).c_str());
                        if (op.is_open())
                            read_for_send(event, m, 0);
                        else
                            status_pro = "404";
                    }
                }
                else if ((file_get != ""))
                {
                    // update changes
                    if (this->Path.find("cgi") != string::npos)
                        find_cgi(config, index_serv); // ila f location dyal cgi kan index file -------------------------
                    else
                    {
                        op.open((this->full_Path).c_str());
                        if (op.is_open())
                            read_for_send(event, m, 0);
                        else
                            status_pro = "404";
                    }
                }
                else if (config[index_serv].get_loc_auto_index(this->Path))
                    root_page(event, this->full_Path);
                else
                    status_pro = "403";
            }
            else
                status_pro = "404";
        }
        else
            redirection_content(event, epoll_fd, config, index_serv);
    }
    else if ((is_open_diir("." + Path) == 0) && (is_open_fil("." + Path) == 1))
    {
        this->full_Path.insert(0, root);
        if (this->Path.find("cgi") != string::npos)
            find_cgi(config, index_serv); // ila f location dyal cgi kan index file -------------------------
        else
        {
            op.open((this->full_Path).c_str());
            if (op.is_open())
                read_for_send(event, m, 0);
            else
                status_pro = "404";
        }
    }
    else
        status_pro = "404";
    close_dir();
}

void Request::Generate_req_second(epoll_event &event, int epoll_fd)
{
    if (op.is_open() && event.events & EPOLLOUT)
    {
        char buf[1024];
        memset(buf, 0, 1024);
        op.read(buf, 1024);

        std::streamsize bytesRead = op.gcount();
        line.append(buf, bytesRead);

        if (buffer != "")
        {
            buffer += line;
            line = buffer;
        }

        stringstream size;
        size_t len;
        string str;
        if (con_type.find("text") != string::npos)
        {
            
            len = line.length();
            len = strlen(line.c_str());
            size << std::hex << len;
            str += size.str();
            str += "\r\n";
            str += line;
            str += "\r\n";
            len = strlen(str.c_str());
            send(event.data.fd, str.c_str(), len, 0);
        }
        else
        {
            len = line.length();
            size << std::hex << len;
            str += size.str();
            str += "\r\n";
            str += line;
            str += "\r\n";
            len = str.length();
            // cout << len << endl;
            cout << send(event.data.fd, str.c_str(), len, 0) << endl;
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
    head = "HTTP/1.1 ";
    head += key;
    head += status;
    head += "\r\n";
    head += "Content-Type: text/html\r\nContent-Lenght:";
    head += oss.str();
    head += "\r\n\r\n";
    head += "<html><head><title>" + status + "</title></head><body><h1>" + key + " " + status + "</h1></body></html>";

    len = head.length();
    line = "";
    this->fin_or_still = finish;
    send(fd, head.c_str(), len, 0);
}

void Request::error_page(epoll_event &event, int epoll_fd, string key, servers &config)
{
    (void)epoll_fd;
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
        ovp.close();
    }
    else
        default_error(key, event.data.fd);
}