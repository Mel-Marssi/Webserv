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
    dire = opendir((Path_tmp.erase(Path_tmp.length() - 1, Path_tmp.length())).c_str());
    if (dire)
    {
        Path_tmp.erase(0, 1);
        struct dirent* entre;
        
        while ((entre = readdir(dire)) != NULL)
        {
            string name = entre->d_name;
            if (name[0] != '.')
            {
                tmp += "<p> <a href= http://localhost:" + _port + Path_tmp;
                tmp += "/" + name + ">" + name +"</a> </p>";
            }
        }
        ss << tmp.length();
        head += ss.str();
        head += "\r\n\r\n";
        head += "<h1> Files Existent on the " + Pat + "</h1>";
        head += tmp;
        head += "</body></html>";
        len = head.length();
        send(event.data.fd, head.c_str(), len, 0);
        fin_or_still = finish;
    }
    else
        status_pro = "404";
}
