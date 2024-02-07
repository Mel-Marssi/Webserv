#include "multiplexing.hpp"
#include "request.hpp"

int Request::check_body_get(epoll_event &event)
{
    if (  ((atol(header_request["Content-Length"].c_str()) > 0) && size_body_get < atol(header_request["Content-Length"].c_str())))
    {   
         size_read_request = -9;
         err = 0;
            int hh = 0;
            char buff[1024];
              if ((event.events & EPOLLIN) && read_get == 1)
             {
                hh = read(event.data.fd, buff, 1024);
                 size_body_get += hh;
              }
            return 1;
        }
    return 0;
}

string Request::resp_post()
{
    string tmp= "HTTP/1.1 201 created\r\nContent-Type:  text/html\r\nContent-Lenght: 790\r\n";
    tmp += "Connection: keep-alive\r\n";
    tmp += "Set-Cookie: " + header_request["Cookie"] + "\r\n\r\n";
    cout << "------> " << header_request["Cookie"] << endl;
    tmp +=  "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>201 Created</title></head><style>";
    tmp += "body{background-color: rgb(247, 253, 247);overflow: hidden;}";
    tmp += "span{display: flex;height:100vh;justify-content: center;align-items: center;}";
    tmp += ".symbol{font-size: 7.5em;color: green;text-shadow: 2px 2px 5px rgb(142, 142, 142);}";
    tmp += " .text{font-size: 3.5em;color: green;text-shadow: 2px 2px 5px rgb(142, 142, 142);}span a{  display: block; color:green;}</style>";
    tmp += "<body><span><label for=\"\" class=\"symbol\">&#10003;</label><label for=\"\" class=\"text\">201 Created</label>";
    if (path_post != "NULL")
    {
        tmp += "<br><a href=\"/";
        tmp += path_post;
        tmp += "\">click here ... </a></span>";
    }
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

void Request::Get_methode(servers &config, epoll_event &event, map<string, string> &m)
{
    if (check_req == 0 && read_get == 1)
    {
        Generate_req_first(event, config, m);
        cgi_handle_get(event, config);
    }
    else if (check_req == 1)
    {
        Generate_req_second(event);
        flg_send = 1;
    }

    read_get = 1;
}

string Request::get_root(servers &config)
{
    string root;
    if (!config[index_serv].get_loc_root(this->Path).empty())
        root = config[index_serv].get_loc_root(this->Path);
    else
        root = config[index_serv].get_root();

    if (root[root.length() - 1] == '/' && full_Path[0] == '/')
        root.erase(root.length() - 1, root.length());
    this->full_Path.insert(0, root);
    return (root);
}