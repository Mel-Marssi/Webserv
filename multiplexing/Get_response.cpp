#include "multiplexing.hpp"
#include "request.hpp"

std::string Request::read_buff(map<string, string> &m, string status)
{
    string head;
    head += "HTTP/1.1 ";
    head += status + get_status_code(status) + "\r\n";
    con_type = "Content-Type: " + get_content_type(m) + "\r\n";
    head += con_type;
    head += "Transfer-Encoding: chunked";
    head += "\r\n\r\n";
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
    memset(buffer, 0, 1024);
    op_cgi.read(buffer, 1024);
    if (op_cgi.bad())
    {
        status_pro = "500";
        return ("");
    }
    //--- size of what read is reading ---
    std::streamsize bytesRead = op_cgi.gcount();
    line.append(buffer, bytesRead);
    //------------------------------------
    head += "HTTP/1.1 200 ok\r\n";
    head += "Content-Lenght:";
    size << fileS; 
    head += size.str();
    head += "\r\n";
    head += line;
    line = "";
    return (head);
}

void Request::read_for_send(epoll_event &event, map<string, string> &m, int flg)
{
    string head;

    if (flg == 0)
    {
        check_req = 1;
        head = read_buff(m, "200");
        buffer += head;
    }
    else if (flg == 1)
    {
        check_req = 1;
        head = read_buff_cgi(m);
        buffer += head;
    }
    if ((event.events & EPOLLOUT) && head != "")
    {
        if (buffer != "")
        {
            len = buffer.length();
            if (send(event.data.fd, buffer.c_str(), len, 0) <= 0)
            {
                status_pro = "500";
            }
            buffer = "";
        }
        else
        {
            len = head.length();
            if (send(event.data.fd, head.c_str(), len, 0) <= 0)
            {
                status_pro = "500";
            }
            buffer = "";
        }
    }
    line = "";
    head = "";
}

void Request::end_of_file(epoll_event &event)
{
    (void)event;
    if (op.is_open())
    {
        string str;
        str += "0\r\n\r\n";
        if (send(event.data.fd, str.c_str(), str.length(), 0) <= 0)
        {
            status_pro = "500";
        }
        fin_or_still = finish;
        op.close();
        close_dir();
    }
    if (op_cgi.is_open())
    {
        fin_or_still = finish;
        op_cgi.close();
        close_dir();
    }
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

int Request::check_permission_R(string str)
{
    if (access((str).c_str(), R_OK) != -1)
        return (1);
    else
        return (0);
}

int Request::is_open_diir(string str)
{
    DIR *d;
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

void Request::response_for_delete(epoll_event &event)
{
    string head;
    size_t len;

    head += "HTTP/1.1 ";
    head += status_pro;
    head += get_status_code(status_pro);
    head += "\r\n\r\n";
    len = head.length();

    if (send(event.data.fd, head.c_str(), len, 0) <= 0)
    {
        status_pro = "500";
    }
}

string Request::get_the_p(servers &config, string Path)
{
    string root = config[index_serv].get_loc_root(this->Path);
    if (root == "")
        root = config[index_serv].get_root();

    if (((Path[0] == '/') && (root[root.length() - 1] == '/')))
        root.erase(root.length() - 1, 1);
    root += Path;
    if (file_get != "")
    {
        if (root[root.length() - 1] != '/')
            root += "/" + file_get;
        else
            root += file_get;
    }
    return (root);
}

void Request::check_url_encoding(string &str)
{
    size_t i = 0;

    for (;;)
    {
        i = str.find("%", i);
        if (i != string::npos)
        {
            string tmp;
            tmp.insert(0, str, i, 3);
            tmp = enco_form_txt(tmp);
            str.erase(i, 3);
            str.insert(i, tmp, 0, tmp.length());           
        }
        else
            break;
        i++;
    }
}

char Request::enco_form_txt(string str)
{
    map<string, string> map_m;
  
    char *endptr;
    int h;
    h = strtol(str.substr(1).c_str(), &endptr, 16);
    return (h);
}


