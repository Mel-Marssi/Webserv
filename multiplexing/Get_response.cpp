#include "multiplexing.hpp"
#include "request.hpp"

std::string Request::read_buff(map<string, string> &m)
{
    string head;
    stringstream size;
    // //to get the size =======
    op.seekg(0, ios::end);
    streampos fileS = op.tellg();
    op.seekg(0, ios::beg);

    char buffer[1024];
    memset(buffer, 0, 1024 );
    op.read(buffer, 1024);
    //--- size of what read is reading ---
    std::streamsize bytesRead = op.gcount();
    line.append(buffer, bytesRead);
    //------------------------------------
    head += "HTTP/1.1 200 ok\r\nContent-Type: ";
    con_type = get_content_type(m);
    head += con_type;
    head += "\r\nContent-Lenght:";
    size << fileS;
    head += size.str();
    head += "\r\n\r\n";
    head += line;
    line = "";
    return (head);
}

std::string Request::read_buff_cgi(map<string, string> &m)
{
    (void)m;
    string head;
    stringstream size;
    // //to get the size =======
    op.seekg(0, ios::end);
    streampos fileS = op.tellg();
    op.seekg(0, ios::beg);

    char buffer[1024];
    memset(buffer, 0, 1024 );
    op.read(buffer, 1024);
    //--- size of what read is reading ---
    std::streamsize bytesRead = op.gcount();
    line.append(buffer, bytesRead);
    //------------------------------------
    head += "HTTP/1.1 200 ok\r\n";
    // con_type = get_content_type(m);
    // head += con_type;
    head += "Content-Lenght:";
    size << fileS; // check this
    head += size.str();
    head += "\r\n\r\n";
    head += line;
    line = "";
    return (head);
}

void Request::read_for_send(map<string, string> &m, int flg)
{
    string head;
    if (flg == 0)
    {
        check_req = 1;
        head = read_buff(m);
        len = head.length();
    }
    else
    {
        check_req = 1;
        
        head = read_buff_cgi(m);
        len = head.length();
    }

    send(event_fd, head.c_str(), len, 0);
    line = "";
}


void Request::end_of_file(epoll_event &event, int epoll_fd)
{
    fin_or_still = finish;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
    close(event_fd);
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

int Request::check_permission(string str)
{
    if (access((str).c_str(), F_OK) != -1)
    {
        if ((access((str).c_str(), R_OK) && access((str).c_str(), W_OK) && access((str).c_str(), X_OK)) == 0)
            return (1);
        else
            return (0);
    }
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