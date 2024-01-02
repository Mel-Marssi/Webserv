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

void Request::read_for_send(map<string, string> &m)
{
    check_req = 1;
    string head = read_buff(m);
    len = head.length();

    send(event_fd, head.c_str(), len, 0);
    line = "";
}

void Request::end_of_file(epoll_event &event, int epoll_fd)
{
    fin_or_still = finish;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event);
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


