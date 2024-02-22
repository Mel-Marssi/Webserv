#include "multiplexing.hpp"
#include "request.hpp"

void Request::root_page(epoll_event &event, string Pat)
{
    string head;
    string tmp;
    stringstream ss;
    string Path_tmp = Pat;

    head += "HTTP/1.1 200 ok\r\nContent-Type: ";
    head += "text/html";
    head += "\r\nContent-Lenght: ";
    close_dir();
    dire = opendir((Path_tmp).c_str()); 
    if (dire)
    {
        Path_tmp.erase(0, 1);
        struct dirent* entre;
        tmp += "<html><head><meta charset=\"UTF-8\"><title> Listed Folder </title><link rel=\"icon\" href=\"/logo.png\" type=\"image/x-icon\"> <link  rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css\"> </head> <body>";
        tmp += "<ul class=\"list-group list-group-flush\">";
        while ((entre = readdir(dire)) != NULL)
        {
            string name = entre->d_name;
            if (name[0] != '.')
            {
                tmp += "<li class=\"list-group-item \"> <a href= \"http://" + _host + ":" + _port + Path_tmp;
                tmp += name + "\">" + name +"</a> </li>";
            }
        }
        ss << tmp.length();
        head += ss.str();
        head += "\r\n\r\n";
        head += "<h1> Files Existent on the " + Pat + "</h1>";
        head += tmp;
        head += "</ul></body></html>";
        len = head.length();
        if (send(event.data.fd, head.c_str(), len, 0) < 0)
            status_pro = "500";
        fin_or_still = finish;
    }
    else
        status_pro = "404";
}



// void Request::Generate_req_second(epoll_event &event)
// {
//     if (op.is_open() && event.events & EPOLLOUT)
//     {
//         char buf[1024];
//         memset(buf, 0, 1024);
//         op.read(buf, 1024);

//         std::streamsize bytesRead = op.gcount();
//         line.append(buf, bytesRead);

//         if (buffer != "")
//         {
//             buffer += line;
//             line = buffer;
//         }

//         stringstream size;
//         size_t len;
//         string str;
//         len = line.length();
//         size << std::hex << len;
//         str += size.str();
//         str += "\r\n";
//         str += line;
//         str += "\r\n";
//         len = str.length();
//         if (send(event.data.fd, str.c_str(), len, 0) < 0)
//         {
//             cout << "SEND ERROR" << endl;
//             status_pro = "500";
//         }
//         line = "";
//         if (op.eof())
//             end_of_file(event);
//     }
//     if (op_cgi.is_open() && event.events & EPOLLOUT)
//     {
//         char buf[1024];
//         memset(buf, 0, 1024);
//         op_cgi.read(buf, 1024);
//         std::streamsize bytesRead = op_cgi.gcount();
//         line.append(buf, bytesRead);

//         if (buffer != "")
//         {
//             buffer += line;
//             line = buffer;
//         }

//         stringstream size;
//         size_t len;
//         string str;
//         len = line.length();
//         size << std::hex << len;
//         str += size.str();
//         str += "\r\n";
//         str += line;
//         str += "\r\n";
//         len = str.length();

//         if (send(event.data.fd, str.c_str(), len, 0) < 0)
//         {
//             cout << "SEND ERROR" << endl;
//             status_pro = "500";
//         }
//         line = "";
//         if (op_cgi.eof())
//             end_of_file(event);
//     }
// }