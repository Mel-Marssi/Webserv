#include "multiplexing.hpp"
#include "request.hpp"

int Request::check_space_first_line()
{
    size_t j = 0;
    int sp = 0;
    while (j < first_line_reque.length())
    {
        if (sp > 2)
        {
            status_pro = "400";
            return 1;
        }
        if (first_line_reque[j] == ' ')
        {
            if (first_line_reque[j + 1] == ' ')
            {
                status_pro = "400";
                return 1;
            }
            sp++;
        }
        j++;
    }
    return 0;
}

int Request::parse_url_prot(string meth, servers &config)
{
    map<string, string>::iterator it;
    size_t i;

    if (status_pro != "NULL")
        return 1;
    //===== tcheck spaces in reques =======
    if (check_space_first_line() == 1)
        return 1;

    it = header_request.find(meth);
    if (it->first == meth)
    {
        string tmp;
        tmp = it->second;
        i = it->second.find("HTTP/1.1");
        if (i == string::npos)
        {
            status_pro = "400";
            return 1;
        }
        this->Path.insert(0, it->second, 0, i - 1);
    }
    if (Path.length() > 2048)
    {
        status_pro = "414";
        return 1;
    }
    check_url_encoding();
    if (meth == "DELETE" && delete_checker(config) == 1)
        return 1;
    i = 1;
    i = this->Path.find("/", i);
    // cout << "Path: " << Path << endl;
    if (Path[i] == '/' && (Path[i + 1] == '/' || Path[i - 1] == '/'))
    {
        status_pro = "404";
        return 1;
    }
    if (i != string::npos)
        handle_Path(i);
    else
        this->full_Path = this->Path;
    return (0);
}

void Request::Generate_req_first(epoll_event &event, servers &config, map<string, string> &m)
{
    if (check_body_get(event) == 1)
        return;

    if (flg_pars_url == 0)
    {
        if (this->parse_url_prot("GET", config) == 1)
            return;
    }

    this->root = get_root(config);
    if ((config[index_serv].get_loc_path_location(this->Path).empty()) && ((is_open_diir("." + Path) == 1)))
        status_pro = "404";
    else if ((!config[index_serv].get_loc_path_location(this->Path).empty()) && (config[index_serv].get_loc_get(this->Path) == 0) && ((is_open_diir("." + Path) == 1)))
        status_pro = "405";
    else if (this->Path == "/")
    {
        // cout << Path << endl;
        string index_file;
        if (!config[index_serv].get_loc_index(Path).empty())
            index_file = config[index_serv].get_loc_index(Path);
        else if (!config[index_serv].get_index().empty())
            index_file = config[index_serv].get_index();
        else
            index_file = "";

        if (!index_file.empty())
        {
            string str1 = handle_Path_location(root, index_file);
            dire = opendir(root.c_str());
            if (dire)
            {
                op.open(str1.c_str());
                if (op.is_open())
                {
                    Path = str1;
                    read_for_send(event, m, 0);
                }
                else
                {
                    string root_tmp = root;
                    if (root_tmp[root_tmp.length() - 1] == '/' && Path[0] == '/')
                        root_tmp.erase(root_tmp.length() - 1, root_tmp.length());
                    if (config[index_serv].get_loc_auto_index(this->Path))
                        root_page(event, root_tmp + Path);
                    else
                        status_pro = "403";
                }
            }
            else
                status_pro = "404";
        }
        else
        {
            if (config[index_serv].get_loc_auto_index(this->Path))
            {
                string tmp = handle_Path_location(root, Path);
                root_page(event, tmp);
            }
            else
                status_pro = "403";
        }
    }
    else if (!(config[index_serv].get_loc_path_location(this->Path).empty()))
    {
        if (config[index_serv].get_loc_redirection(this->Path) == "")
        {
            string tmp = handle_Path_location(root, Path);
            dire = opendir(tmp.c_str());
            if (dire)
            {
                if ((file_get == "") && (this->Path_bef[Path_bef.length() - 1] != '/'))
                    redirection_content(event, config, "301", 1);
                else if ((file_get == "") && (!(config[index_serv].get_loc_index(this->Path).empty())))
                {
                    if (this->Path.find("cgi") != string::npos && flag_read_cgi == 1)
                    {
                        file_get = config[index_serv].get_loc_index(this->Path);
                        find_cgi(config, index_serv);
                    }
                    if (flag_read_cgi == 0 || this->Path.find("cgi") == string::npos)
                    {
                        this->Path = this->full_Path + config[index_serv].get_loc_index(this->Path);
                        check_files_open(event, m, Path);
                    }
                }
                else if ((file_get != ""))
                {
                    if (this->Path.find("cgi") != string::npos && flag_read_cgi == 1)
                        find_cgi(config, index_serv);
                    if (flag_read_cgi == 0 || this->Path.find("cgi") == string::npos)
                        check_files_open(event, m, this->full_Path);
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
            redirection_content(event, config, "301", 0);
    }
    else if ((is_open_diir("." + Path) == 0) && (is_open_fil("." + Path) == 1))
        check_files_open(event, m, this->full_Path);
    else
        status_pro = "404";
    close_dir();
    check_req = 1;
}

void Request::Generate_req_second(epoll_event &event)
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
        len = line.length();
        size << std::hex << len;
        str += size.str();
        str += "\r\n";
        str += line;
        str += "\r\n";
        len = str.length();
        if (send(event.data.fd, str.c_str(), len, 0) < 0)
            status_pro = "500";
        line = "";
        if (op.eof())
            end_of_file(event);
    }
    if (op_cgi.is_open() && event.events & EPOLLOUT)
    {
        char buf[1024];
        memset(buf, 0, 1024);
        op_cgi.read(buf, 1024);
        std::streamsize bytesRead = op_cgi.gcount();
        line.append(buf, bytesRead);

        if (buffer != "")
        {
            buffer += line;
            line = buffer;
        }

        stringstream size;
        size_t len;
        string str;
        len = line.length();
        size << std::hex << len;
        str += size.str();
        str += "\r\n";
        str += line;
        str += "\r\n";
        len = str.length();

        if (send(event.data.fd, str.c_str(), len, 0) < 0)
            status_pro = "500";
        line = "";
        if (op_cgi.eof())
            end_of_file(event);
    }
}

void Request::default_error(string key, int fd)
{
    string head;
    string status = get_status_code(key);
    int size = 19 + status.length() + 25 + key.length() + 1 + status.length() + 19;
    std::ostringstream oss;
    oss << size;
    head = "HTTP/1.1 ";
    head += key;
    head += status;
    if (methode != "HEAD")
    {
        head += "\r\n";
        head += "Content-Type: text/html\r\nContent-Lenght:";
        head += oss.str();
    }
    head += "\r\n\r\n";
    if (methode != "HEAD")
    {
        head += "<html><head><title>" + status + "</title></head><body><h1>" + key + " " + status + "</h1></body></html>";
    }
    len = head.length();
    line = "";
    this->fin_or_still = finish;
    if (send(fd, head.c_str(), len, 0) < 0)
        status_pro = "500";
}

void Request::error_page(epoll_event &event, string key, servers &config)
{
    close_dir();
    string str = config[index_serv]._error_book[atoi(key.c_str())];
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
        if (send(event.data.fd, head.c_str(), len, 0) < 0)
            status_pro = "500";
        end_of_file(event);
        ovp.close();
    }
    else
        default_error(key, event.data.fd);
}