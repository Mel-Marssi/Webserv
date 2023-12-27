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
        this->Path.erase(i, this->Path.length());
        this->full_Path = this->Path;
    }
    else
        this->full_Path = this->Path;
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
        this->Port.insert(0, it->second, b + 1, it->second.length());
 
    }
}

std::string Request::read_buff(map<string, string> &m)
{
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
    head += fileSize.str();
    head += "\r\n\r\n";
    head += line;
    return (head);
}
// #include "/pars_config/servers.hpp"
void Request::Generate_req_first(epoll_event &event, servers &config, int epoll_fd, map<string, string> &m)
{
    this->parse_url_prot();
    cout << Port.c_str() << endl;
   cout << get_right_index(config.server, (int)atoi(Port.c_str()),Host, server_name) <<endl;
    string root = config.get_loc_root(this->Path);
    if (root[root.length() - 1] == '/')
        this->full_Path.erase(0,1);

    this->full_Path.insert(0, root);
// cout << this->full_Path << "\t" << this->file_get << "\t" << this->Path <<"\t===============\n";
    if (config.get_loc_get(this->Path) && (config.get_loc_root(this->Path) != ""))
    {
        if (config.get_loc_redirection(this->Path) == "")
        {
            string str = this->Path;
            dire = opendir(this->full_Path.c_str());
            if (dire)
            {
                op.open((this->full_Path + "/" +this->file_get).c_str());
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
                    // cout << "respons=-------------------\n";

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
                if (dire)
                    {closedir(dire), dire = NULL;}
                error_page(event, epoll_fd, "404");
            }
        }
        else
            redirection_content(event, epoll_fd, config);
    }
    else if (config.get_loc_get(this->Path))
    {
        // cout << "==================\n";
        this->Path = config.get_root(config[0].get_port());
        this->file_get = config.get_index(config[0].get_port());

        dire = opendir(Path.c_str());
        if (dire)
        {
            string str;
            if (Path[Path.length() - 1] == '/')
                str = Path + file_get;
            else
                str = Path + "/" + file_get;
            op.open(str.c_str());
            if (op.is_open())
            {
                    // cout << "respons=-------------------\n";
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
            error_page(event, epoll_fd, "404");
    }
    else
        error_page(event, epoll_fd, "404");
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
                    // cout << "respons=-------------------\n";
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
    std::ifstream ovp("./prblm/file_not_found.html");
    if (ovp.is_open() && fin_or_still != finish)
    {
        string head;
        getline(ovp, line, '\0');
        size_t len = line.length();
        // head += "HTTP/1.1 404 Not Found\r\nContent-Type:  text/html\r\nContent-Lenght:";
        head += "HTTP/1.1 ";
        head += key;
        head += get_status_code(key);
        head += "\r\n";
        head += "Content-Type:  text/html\r\nContent-Lenght:";
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
        // cout << "respons=-------------------\n";
        if (dire != NULL)
        {
            closedir(dire);
            dire = NULL;
        }
    }
}