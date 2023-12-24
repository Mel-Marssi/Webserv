#include "multiplexing.hpp"
#include "request.hpp"

void multiplexing::fill_content_type()
{
    cont_type["txt"] = "text/plain";
    cont_type["html"] = "text/html";
    cont_type["htm"] = "text/html";
    cont_type["jpg"] = "image/jpeg";
    cont_type["jpeg"] = "image/jpeg";
    cont_type["png"] = "image/png";
    cont_type["gif"] = "image/gif";
    cont_type["mp3"] = "audio/mpeg";
    cont_type["mp4"] = "video/mp4";
    cont_type["webm"] = "video/webm";
    cont_type["pdf"] = "application/pdf";
    cont_type["zip"] = "application/zip";
}

string Request::get_content_type(map<string, string>& m)
{
    map<string, string>::iterator it;

    size_t s = this->Path.find(".");
    s++;
    exten.insert(0, this->Path, s, this->Path.length());

    it = m.find(exten);
    if (it != m.end())
        return (it->second);
    return ("NULL");
}

void Request::redirection_content(epoll_event &event, int epoll_fd, servers &config)
{
    string head;
    head += "HTTP/1.1 301 Moved Permanently\r\nLocation: ";
    head += config.get_loc_redirection(this->Path);
    head += "\r\n\r\n";
    
    size_t len = head.length();
    send(event.data.fd, head.c_str(), len, 0);
    fin_or_still = finish;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
    close(event.data.fd);
    op.close();
    closedir(dire);
    dire = NULL;
}