#include "multiplexing.hpp"
#include "request.hpp"

int Request::check_body_get(epoll_event &event)
{
	if ((fir_body != "NULL" || atol(header_request["Content-Length"].c_str()) > 0) && status_pro != "NULL" )
    {
        size_read_request = -9;
        err = 0;
        int hh = 0;
        char buff[1024];
        if (event.events & EPOLLIN && read_get == 1)
            hh = read(event.data.fd, buff, 1024);
        size_body_get += hh;
        read_request.clear();
        read_request.append(buff,hh);
        if (size_body_get >= atol(header_request["Content-Length"].c_str()))
            finir = 1;
   
        return 1;
    }
    else  if (((fir_body != "NULL" && fir_body != "0\r\n\r\n") || atol(header_request["Content-Length"].c_str()) > size_body_get) && status_pro == "NULL")
    {   
        size_read_request = -9;
 
        err = 0;
        int hh = 0;
        if ((event.events & EPOLLIN)  )
        {
            char buff[1024];
            if (size_body_get + 1024 <= atol(header_request["Content-Length"].c_str()))
            {
                hh = read(event.data.fd, buff, 1024);
                 if (hh < 0)
                    exit(1);
            size_body_get += hh;
            read_request.clear();
            read_request.append(buff,hh);

            }
            else
            {
                hh = read(event.data.fd, buff, atol(header_request["Content-Length"].c_str()) - size_body_get);
                if (hh < 0)
                    exit(1);
                fir_body = "NULL";
                size_body_get += hh;
                read_request.clear();
                read_request.append(buff,hh);
                return 0;
            }
            return 1;
        }
        if  (size_body_get < atol(header_request["Content-Length"].c_str()) && atol(header_request["Content-Length"].c_str()) > 0)
        {
            return 1;
        }
    }
	return 0;
}

string Request::resp_post()
{
    string tmp= "HTTP/1.1 201 created\r\nContent-Type:  text/html\r\nContent-Lenght: 790\r\n\r\n";
    tmp +=  "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>201 Created</title></head><style>";
    tmp += "body{background-color: rgb(247, 253, 247);overflow: hidden;}";
    tmp += "span{display: flex;height:100vh;justify-content: center;align-items: center;}";
    tmp += ".symbol{font-size: 7.5em;color: green;text-shadow: 2px 2px 5px rgb(142, 142, 142);}";
    tmp += " .text{font-size: 3.5em;color: green;text-shadow: 2px 2px 5px rgb(142, 142, 142);}span a{  display: block; color:green;}</style>";
    tmp += "<body><span><label for=\"\" class=\"symbol\">&#10003;</label><label for=\"\" class=\"text\">201 Created</label>";
    tmp += "<br><a href=\"/";
    tmp += path_post;
    tmp += "\">click here ... </a></span>";
    tmp += "</body></html>";
    return tmp;
}

void Request::check_files_open(epoll_event &event, map<string, string> m, string str)
{
    op.open(str.c_str());
    if (op.is_open())
        read_for_send(event, m, 0);
    else
        status_pro = "404";
}

void Request::Get_methode(servers &config, epoll_event &event, map<string, string> &m, int epoll_fd)
{
    if (check_req == 0 && read_get == 1)
    {
        Generate_req_first(event, config, m);
        cgi_handle_get(epoll_fd, event, config);
    }
    else if (check_req == 1)
    {
        Generate_req_second(event, epoll_fd, m);
        flg_send = 1;
    }

    read_get = 1;
}