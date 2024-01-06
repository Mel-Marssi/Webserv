#include "multiplexing.hpp"
#include "request.hpp"

void multiplexing::fill_content_type()
{
    cont_type["txt"] = "text/plain";
    cont_type["cpp"] = "text/plain";
    cont_type["hpp"] = "text/plain";
    cont_type["o"] = "text/html";
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

void Request::fill_status_code()
{
    //===================== status ====================
    status_code["404"] = " Not Found";
    status_code["200"] = " OK";
    status_code["301"] = " Moved Permanently";
    status_code["403"] = " Forbidden";
    status_code["400"] = " Bad Request";
    status_code["405"] = " Method Not Allowed";
    //=================== DELETE ==================
    status_code["409"] = " Conflict";
    status_code["204"] = " No Content";
    status_code["500"] = " Internal Server Error";
}

string Request::get_status_code(string key)
{
    map<string, string>::iterator it;
    it = status_code.find(key);
    if (it != status_code.end())
        return (it->second);
    return ("NULL");
}

string Request::get_content_type(map<string, string>& m)
{
    map<string, string>::iterator it;
    size_t s;
    if (file_get != "")
    {
        s = this->file_get.find(".");
        s++;
        exten.insert(0, this->file_get, s, this->file_get.length());
    }
    else
    {
        s = this->Path.find(".");
        s++;
        exten.insert(0, this->Path, s, this->Path.length());
    }
    // s++;
    // exten.insert(0, this->file_get, s, this->file_get.length());
    it = m.find(exten);
    if (it != m.end())
        return (it->second);
    return ("NULL");
}

void Request::redirection_content(epoll_event &event, int epoll_fd, servers &config , int index)
{
    string head;
    head += "HTTP/1.1 301 Moved Permanently\r\nLocation: ";
    head += config[index].get_loc_redirection(this->Path);
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

void Request::redirection_content_backSlash(epoll_event &event, int epoll_fd, int flg)
{
    if (flg == 0)
    {
        string head;
        head += "HTTP/1.1 301 Moved Permanently\r\nLocation: ";
        head += this->Path + "/";
        head += "\r\n\r\n";
        
        size_t len = head.length();
        send(event.data.fd, head.c_str(), len, 0);
        fin_or_still = finish;
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
        close(event.data.fd);
        op.close();
        close_dir();
    }
    else
    {
        string head;
        head += "HTTP/1.1 409 Conflict\r\nLocation: ";
        head += this->Path + "/";
        head += "\r\n\r\n";
        
        size_t len = head.length();
        send(event.data.fd, head.c_str(), len, 0);
        fin_or_still = finish;
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
        close(event.data.fd);
        op.close();
        close_dir();
    }
}