#include "multiplexing.hpp"
#include "request.hpp"

void Request::root_page(epoll_event &event, int epoll_fd, servers &config)
{
    string head;
    head += "HTTP/1.1 200 ok\r\nContent-Type: ";
    head += "text/html";
    head += "\r\nContent-Lenght: 150";
    head += "\r\n\r\n";
    ifstream opp("./root_page/root.html");

    if (opp.is_open())
    {
        char buffer[1024];
        opp.read(buffer, 1024);
        head += buffer;

        dire = opendir(".");
        if (dire)
        {
            struct dirent* entre;
            
            while ((entre = readdir(dire)) != NULL)
            {
                string name = entre->d_name;
                if (name[0] != '.')
                {
                    size_t s = name.find(".");
                    if (s == string::npos)
                    {
                        head += "<p> <a href=https:www.youtube.com>"+ name +"</a> </p>";
                    }
                    else
                    {
                        head += "<p> <a href=/test/fich.pdf>"+ name +"</a> </p>";
                    }
                }
            }
            len = head.length();
            // cout << head << "============="<< endl;
            send(event.data.fd, head.c_str(), len, 0);
        }
        
        // cout << "dkjoooooooooooooool\n";
        fin_or_still = finish;
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
        close(event.data.fd);
        opp.close();
        closedir(dire);
        dire = NULL;
        (void)config;
        (void)epoll_fd;
    }
    else
        cout << "File Madkhelx\n";
    
    // (void)event;
    // (void)config;
        // cout << "none" << endl;
}
