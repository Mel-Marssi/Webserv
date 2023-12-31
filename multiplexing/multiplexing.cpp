#include "multiplexing.hpp"
#include "request.hpp"

multiplexing::multiplexing(servers &config)
{
	int server_socket[config.size()];
	struct sockaddr_in adress;
	int wait_fd, set_socket = 1;
	int fd_client;
	int epoll_fd = epoll_create(1);
	if ( epoll_fd < 0)
		throw (runtime_error("epoll_create() call failed!"));
	struct epoll_event event, event_wait[1024];

	for (int i = 0; i < config.size(); i++)
	{
		stringstream int_to_string[2];
		if ((server_socket[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			throw (runtime_error("socket() call failed!"));
		adress.sin_addr.s_addr = inet_addr(config[i].get_host().c_str());
		adress.sin_family = AF_INET;
		adress.sin_port = htons(config[i].get_port());
		int_to_string[0] << config[i].get_port();
		int_to_string[1] << config[i].get_host();
		server_book[server_socket[i]] = make_pair(int_to_string[0].str(), int_to_string[1].str());

		if (setsockopt(server_socket[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &set_socket, sizeof(set_socket)) < 0)
			throw(runtime_error("setsockopt() call failed!"));
		if (bind(server_socket[i], (const sockaddr *)&adress, sizeof(adress)) < 0)
			throw(runtime_error("bind() call failed!"));
		if (listen(server_socket[i], 0) < 0)
			throw(runtime_error("listen() call failed!"));
		event.data.fd = server_socket[i];
		event.events =  EPOLLIN | EPOLLOUT;
		// cout << "-=----> " << event.data.fd <<endl;
		if (epoll_ctl(epoll_fd,EPOLL_CTL_ADD, server_socket[i], &event) < 0)
			throw(runtime_error("epoll_ctl() call failed!"));
	}
	//Methode GET fill Content Type : =======================
	this->fill_content_type();
	//================================================
	for (;;)
	{
		wait_fd = epoll_wait(epoll_fd, event_wait, 1024, 0);
		for(int i = 0; i < wait_fd ; i++)
		{
			if(event_wait[i].data.fd <= server_socket[config.size() - 1])
			{
				server_book[event_wait[i].data.fd];
				if ((fd_client = accept(event_wait[i].data.fd, NULL, NULL)) < 0)
					continue;
				event.data.fd = fd_client;
				event.events =  EPOLLIN | EPOLLOUT;
				server_book[fd_client] = server_book[event_wait[i].data.fd];
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_client, &event);
				request.insert(std::make_pair(fd_client, Request(server_book, fd_client)));
			}
			else
			{
				// char buff[1024];
				// for(int i=0; i < 1024;i++)
				// 	buff[i] = 0;
				// int size = 0;
				// if (request[event_wait[i].data.fd].kk == 0)
				// {
				// 	request[event_wait[i].data.fd].kk = 1;
				// 	if ((size = read(event_wait[i].data.fd, buff, 1024)) == 0)
				// 		break ;
				// 	request[event_wait[i].data.fd].size_read_request += size;
				// 	request[event_wait[i].data.fd].read_request.append(buff,size);
				// 	request[event_wait[i].data.fd].parce_request(request[event_wait[i].data.fd].read_request);
				// }
				
				if (request[event_wait[i].data.fd].check_left_header == 0)
				{
					char buff[1024];
	 				// cout << "dddd\n";
					request[event_wait[i].data.fd].size = 0;
					request[event_wait[i].data.fd].size = read(event_wait[i].data.fd, buff, 1024);
					request[event_wait[i].data.fd].size_read_request += request[event_wait[i].data.fd].size;
					request[event_wait[i].data.fd].read_request.append(buff,request[event_wait[i].data.fd].size);
					request[event_wait[i].data.fd].parce_request(request[event_wait[i].data.fd].read_request);
				}

				if (request[event_wait[i].data.fd].methode == "POST")
				{	
 
					request[event_wait[i].data.fd].post(event_wait[i].data.fd);
		 
				}
				else if (request[event_wait[i].data.fd].methode == "GET" && request[event_wait[i].data.fd].fin_or_still == Still)
				{
					request[event_wait[i].data.fd]._port = server_book[event_wait[i].data.fd].first;
  				    request[event_wait[i].data.fd]._host = server_book[event_wait[i].data.fd].second;
					request[event_wait[i].data.fd].event_fd = event_wait[i].data.fd;
					// cout << "----->" << request[event_wait[i].data.fd]._port << " " << request[event_wait[i].data.fd]._host <<endl;
					if (request[event_wait[i].data.fd].check_req == 0)
					{
						request[event_wait[i].data.fd].fill_status_code();
						request[event_wait[i].data.fd].Generate_req_first(event_wait[i], config, epoll_fd, cont_type);
					}
					else if (request[event_wait[i].data.fd].check_req == 1)
						request[event_wait[i].data.fd].Generate_req_second(event_wait[i], epoll_fd);
					if (request[event_wait[i].data.fd].fin_or_still == finish)
					{
						std::map<int, Request>::iterator it = request.find(event_wait[i].data.fd);
						if (it != request.end())
							request.erase(it);
						// request[event_wait[i].data.fd].read_request = "";
						request[event_wait[i].data.fd].read_request.erase();
					}
					// cout << request[event_wait[i].data.fd].header_request << endl;
				}
				if (request[event_wait[i].data.fd].methode == "NONE")
				{
					request[event_wait[i].data.fd].error_page(event_wait[i], epoll_fd, "400");
					std::map<int, Request>::iterator it = request.find(event_wait[i].data.fd);
						if (it != request.end())
							request.erase(it);
					request[event_wait[i].data.fd].read_request = "";
				}
				if ( request[event_wait[i].data.fd].size_request < request[event_wait[i].data.fd].size_read_request || request[event_wait[i].data.fd].finir == 1)
				{
 
					const char n[170] = "HTTP/1.1 200 ok\r\nContent-Type:  text/html\r\nContent-Lenght:19\r\n\r\n <html><head><title>Hello Page</title></head><body><h1>Hello, client!</h1></body></html>";
					send(event_wait[i].data.fd, n, 170, 0);
					close(event_wait[i].data.fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_wait[i].data.fd, &event_wait[i]);
					std::map<int, Request>::iterator it = request.find(event_wait[i].data.fd);
					if (it != request.end())
						request.erase(it);
				}
				// else
				request[event_wait[i].data.fd].read_request.erase();

			}
			}
		}
}


multiplexing::~multiplexing()
{
}