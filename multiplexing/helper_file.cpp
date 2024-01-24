#include "multiplexing.hpp"
#include "request.hpp"

// int Request::check_body_get(epoll_event &event)
// {
//     if ((fir_body != "NULL" || atol(header_request["Content-Length"].c_str()) > 0) && status_pro != "NULL" )
//     {   
//         size_read_request = -9;
//         err = 0;
//         int hh = 0;
//         char buff[1024];
//         if (event.events & EPOLLIN)
//             hh = read(event.data.fd, buff, 1024);
//         size_body_get += hh;
//         read_request.clear();
//         read_request.append(buff,hh);
//         if ( size_body_get >= atol(header_request["Content-Length"].c_str()))
//             finir = 1;
//         return 1;
//     }
//     else  if ((fir_body != "NULL" || atol(header_request["Content-Length"].c_str()) > 0) && status_pro == "NULL")
//     {   
//         size_read_request = -9;
//         err = 0;
//         int hh = 0;
//         char buff[1024];
//         if (event.events & EPOLLIN)
//         {
//             hh = read(event.data.fd, buff, 1024);
//             size_body_get += hh;
//             read_request.clear();
//             read_request.append(buff,hh);
//             return 1;
//         }
//     }
// 	return (0);
// }

int Request::check_body_get(epoll_event &event)
{
	if ((fir_body != "NULL" || atol(header_request["Content-Length"].c_str()) > 0) && status_pro != "NULL" )
    {   
        cout << "9999\n";

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
        cout << "888\n";
       
        size_read_request = -9;
 
        err = 0;
        int hh = 0;
        char buff[1024];
        if ((event.events & EPOLLIN)  )
        {
        cout << "fffff\n";
            hh = read(event.data.fd, buff, 1024);
        cout << "hhhh\n";
            size_body_get += hh;
            read_request.clear();
            read_request.append(buff,hh);
            return 1;
        }
        if  (size_body_get < atol(header_request["Content-Length"].c_str()) && atol(header_request["Content-Length"].c_str()) > 0)
        {
         return 1;
        }

    }
	return 0;
}