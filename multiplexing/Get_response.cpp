#include "multiplexing.hpp"
#include "request.hpp"

// std::string Request::read_buff(map<string, string> &m)
// {
//     string head;
//     stringstream size;
//     // //to get the size =======
//     op.seekg(0, ios::end);
//     streampos fileS = op.tellg();
//     op.seekg(0, ios::beg);

//     char buffer[5000];
//     memset(buffer, 0, 5000 );
//     op.read(buffer, 5000);
//     //--- size of what read is reading ---
//     std::streamsize bytesRead = op.gcount();
//     line.append(buffer, bytesRead);
//     //------------------------------------
//     head += "HTTP/1.1 200 ok\r\nContent-Type: ";
//     con_type = get_content_type(m);
//     head += con_type;
//     head += "\r\nContent-Lenght:";
//     size << fileS;
//     head += size.str();
//     head += "\r\n\r\n";
//     head += line;
//     line = "";
//     return (head);
// }

std::string Request::read_buff(map<string, string> &m)
{
    string head;
    // stringstream size;
    // stringstream sizee;
    // //to get the size =======
    // op.seekg(0, ios::end);
    // streampos fileS = op.tellg();
    // op.seekg(0, ios::beg);
    // startTime = clock();

    // char buffer[1024];
    // memset(buffer, 0, 1024 );
    // op.read(buffer, 1024);
    //--- size of what read is reading ---
    // std::streamsize bytesRead = op.gcount();
    // line.append(buffer, bytesRead);
    //------------------------------------
    head += "HTTP/1.1 200 OK\r\n";
    con_type = "Content-Type: " + get_content_type(m) + "\r\n";
    head += con_type;
    // size << std::hex << fileS;
    head += "Transfer-Encoding: chunked";
    head += "\r\n\r\n";
    //========================
    // sizee << std::hex << line.length();
    // head += sizee.str() + "\r\n";
    //========================
    // head += line;
    // head += "\r\n";
    // line = "";
    // cout << head << endl;
    return (head);
}

std::string Request::read_buff_cgi(map<string, string> &m)
{
    (void)m;
    string head;
    stringstream size;
    // //to get the size =======
    op_cgi.seekg(0, ios::end);
    streampos fileS = op_cgi.tellg();
    op_cgi.seekg(0, ios::beg);

    char buffer[1024];
    memset(buffer, 0, 1024 );
    op_cgi.read(buffer, 1024);
    //--- size of what read is reading ---
    std::streamsize bytesRead = op_cgi.gcount();
    line.append(buffer, bytesRead);
    //------------------------------------
    head += "HTTP/1.1 200 ok\r\n";
    // con_type = get_content_type(m);
    // head += con_type;
    head += "Content-Lenght:";
    size << fileS; // check this
    head += size.str();
    head += "\r\n";
    // head += "\r\n\r\n";
    head += line;
    line = "";
    return (head);
}

void Request::read_for_send(epoll_event &event, map<string, string> &m, int flg)
{
    string head;
    // if (read_buuf_for_get(event);
//  cout << "4444\n";

    // if ( size_body_get >= atol(header_request["Content-Length"].c_str()))
    // {
    //     cout << "Fffffffefe\n";
    //     fir_body = "NULL";
    // }
    // else if (!(event.events & EPOLLIN))
    //     return;
    // if (fir_body != "NULL" && (event.events & EPOLLIN) && check_read_get == 1)
    // {
    //     int hh = 0;
    //     char buff[1024];
    //     cout << "999999999999\n";
    //     hh = read(event.data.fd, buff, 1024);
    //     size_body_get += hh;
    //     fir_body = "oo";
    //     return;
    // }
    if (flg == 0)
    {
        check_req = 1;
        head = read_buff(m);
        buffer += head;
    }
    else if (flg == 1)
    {
        check_req = 1;
        head = read_buff_cgi(m);
        buffer += head;
    }

    if ((event.events & EPOLLOUT))
    {
        if (buffer != "")
        {
            len = buffer.length();
            send(event.data.fd, buffer.c_str(), len, 0);
            buffer = "";
        }
        else
        {
            len = head.length();
            send(event.data.fd, head.c_str(), len, 0);
            buffer = "";
        }
    }
    line = "";
    head = "";
}
// void Request::read_for_send(map<string, string> &m, int flg)
// {
//     string head;
//     if (flg == 0)
//     {
//         check_req = 1;
//         head = read_buff(m);
//         len = head.length();
//         buffer = head;
//         if (con_type.find("text") != string::npos)
//             len = strlen(buffer.c_str());
//         else
//             len = buffer.length();
//     }
//     else
//     {
//         check_req = 1;
        
//         head = read_buff_cgi(m);
//         len = head.length();
//         buffer = head;
//     }
 
//     send(event_fd, head.c_str(), len, 0);
//     line = "";
// }


void Request::end_of_file(epoll_event &event, int epoll_fd)
{
    (void)epoll_fd;
    string str;
    str += "0\r\n\r\n";
    send(event.data.fd, str.c_str(), str.length(), 0);
    fin_or_still = finish;
    // close(event.data.fd);
    // epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
    op.close();
    close_dir();
}

void Request::close_dir()
{
    if (dire)
    {
        closedir(dire);
        dire = NULL;
    }
}

int Request::check_permission_F(string str)
{
    if (access((str).c_str(), F_OK) != -1)
        return (1);
    return (0);
}

int Request::check_permission_X(string str)
{
    if (access((str).c_str(), X_OK) != -1)
        return (1);
    else
        return (0);
}

int Request::is_open_diir(string str)
{
    DIR* d;
    d = opendir(str.c_str());
    if (d)
    {
        closedir(d);
        d = NULL;
        return 1;
    }
    return 0;
}

int Request::is_open_fil(string str)
{
    ifstream fil(str.c_str());
    if (fil.is_open())
    {
        fil.close();
        return 1;
    }
    return 0;
}

void Request::response_for_delete(string status, epoll_event &event, int epoll_fd)
{
    string head;
    size_t len;

    head += "HTTP/1.1 ";
    head += status;
    head += get_status_code(status);
    head += "\r\n\r\n";
    len = head.length();

    send(event.data.fd, head.c_str(), len, 0);
    fin_or_still = finish;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
    close(event.data.fd);
}

string Request::get_the_p(servers &config, string Path, string file_get)
{
    (void)file_get;
	int indx = get_right_index(config.server, atoi(_port.c_str()), _host, config.get_server_name(atoi(_port.c_str())));

	string root = config[indx].get_loc_root(this->Path);
	if (root == "")
		root = config[indx].get_root();
    
    if (((Path[0] == '/') && (root[root.length() - 1] == '/')))
        root.erase(root.length() - 1, 1);

    return (root);
}

void Request::check_url_encoding()
{
    size_t i = 0;
    for (;;)
    {
        i = Path.find("%");
        if (i != string::npos)
        {
            string tmp;
            tmp.insert(0, Path, i, 3);
            tmp = enco_form_txt(tmp);
            Path.erase(i, 3);
            Path.insert(i, tmp, 0, tmp.length());
        }
        else
            break;
    }
}

string Request::enco_form_txt(string str)
{
    map<string, string> map;
    map["%21"] = "!";
    map["%23"] = "#";
    map["%24"] = "$";
    map["%26"] = "&";
    map["%27"] = "'";
    map["%28"] = "(";
    map["%29"] = ")";
    map["%2A"] = "*";
    map["%2B"] = "+";
    map["%2C"] = ",";
    map["%2F"] = "/";
    map["%3A"] = ":";
    map["%3B"] = ";";
    map["%3D"] = "=";
    map["%3F"] = "?";
    map["%40"] = "@";
    map["%5B"] = "[";
    map["%5D"] = "]";
    map["%20"] = " ";
    map["%22"] = "\"";
    map["%25"] = "%";
    map["%2D"] = "-";
    map["%2E"] = ".";
    map["%3C"] = "<";
    map["%3E"] = ">";
    map["%5C"] = "\\";
    map["%5E"] = "^";
    map["%5F"] = "_";
    map["%60"] = "`";
    map["%7B"] = "{";
    map["%7C"] = "|";
    map["%7D"] = "}";
    map["%7E"] = "~";

    return (map[str]);
}

void Request::read_buuf_for_get(epoll_event &event)
{
    if ( size_body_get >= atol(header_request["Content-Length"].c_str()))
        fir_body = "NULL";
    else if (!(event.events & EPOLLIN))
        return ;
    if (fir_body != "NULL" && (event.events & EPOLLIN) && check_read_get == 1)
    {
        int hh = 0;
        char buff[1024];
        cout << "999999999999\n";
        hh = read(event.data.fd, buff, 1024);
        cout << "111111111111111\n";
        size_body_get += hh;
        fir_body = "oo";
    }
    return ;
}