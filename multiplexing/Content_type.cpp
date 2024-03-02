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
    cont_type["php"] = "application/x-httpd-php";
    cont_type["tar"] = "application/x-tar";
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
    status_code["504"] = " Gateway Timeout";
    status_code["413"] = " Request Entity Too Large";
    status_code["501"] = " Not Implemented";
    status_code["414"] = " Request-URI Too Long";
    status_code["415"] = " Unsupported Media Type";  
    status_code["411"] = " Length Required";
    //=================== DELETE ==================
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
        s = this->Path.find(".", 2);
        s++;
        exten.insert(0, this->Path, s, this->Path.length());
    }
    it = m.find(exten);
    if (it != m.end())
        return (it->second);
    return ("text/plain");
}

void Request::redirection_content(epoll_event &event, servers &config, string str, int flg)
{
    string head;
    head += "HTTP/1.1 ";
    head += str + get_status_code(str) + "\r\nLocation: ";

    if (flg == 0)
        head += config[index_serv].get_loc_redirection(this->Path);
    else
        head += this->Path + "/";
    head += "\r\n\r\n";
    size_t len = head.length();
    if (send(event.data.fd, head.c_str(), len, 0) <= 0)
        status_pro = "500";
    fin_or_still = finish;
    close_dir();
}