#include "multiplexing.hpp"
#include "request.hpp"

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


