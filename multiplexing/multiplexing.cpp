#include "multiplexing.hpp"
#include "request.hpp"

multiplexing::multiplexing(servers &config)
{
	int server_socket[config.size()];
	struct sockaddr_in adress;
	int wait_fd, set_socket = 1;
	int fd_client;
	int epoll_fd = epoll_create(1);
	// size_read_request = 0;
	if ( epoll_fd < 0)
		throw (runtime_error("epoll_create() call failed!"));
	struct epoll_event event, event_wait[1024];

	for (int i = 0; i < config.size(); i++)
	{
		if ((server_socket[i] = socket(AF_INET,SOCK_STREAM, 0)) < 0)
			throw (runtime_error("socket() call failed!"));
		adress.sin_addr.s_addr = INADDR_ANY;
		adress.sin_family = AF_INET;
		adress.sin_port = htons(config[i].get_port());

		if (setsockopt(server_socket[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &set_socket, sizeof(set_socket)) < 0)
			throw(runtime_error("setsockopt() call failed!"));
		if (bind(server_socket[i], (const sockaddr *)&adress, sizeof(adress)) < 0)
			throw(runtime_error("bind() call failed!"));
		if (listen(server_socket[i], 0) < 0)
			throw(runtime_error("listen() call failed!"));
		event.data.fd = server_socket[i];
		event.events =  EPOLLIN | EPOLLOUT;
		if (epoll_ctl(epoll_fd,EPOLL_CTL_ADD, server_socket[i], &event) < 0)
			throw(runtime_error("epoll_ctl() call failed!"));
	}

	for (;;)
	{
		wait_fd = epoll_wait(epoll_fd, event_wait, 1024, 0);
		for(int i = 0; i < wait_fd ; i++)
		{
			if(event_wait[i].data.fd <= server_socket[config.size() - 1])
			{
				if ((fd_client = accept(event_wait[i].data.fd, NULL, NULL)) < 0)
					continue;
				event.data.fd = fd_client;
				event.events =  EPOLLIN | EPOLLOUT;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_client, &event);
				request.insert(std::make_pair(fd_client, Request()));
				// std::string read_request = "NONE";
				//request[fd_client].parce_request(read_request);
			}
			else
			{
				char buff[1024];
				// std::string read_request;
				int size = 0;
				size = read(event_wait[i].data.fd, buff, 1024);
				request[event_wait[i].data.fd].size_read_request += size;
				request[event_wait[i].data.fd].read_request.append(buff,size);
				request[event_wait[i].data.fd].parce_request(request[event_wait[i].data.fd].read_request);
				// std::cout << request[event_wait[i].data.fd].header_request["Content-Length"] <<std::endl;
				// cout << "----"<< request[event_wait[i].data.fd].read_request;

				if (request[event_wait[i].data.fd].methode == "POST")
				{	
					if (request[event_wait[i].data.fd].check_create_file == 0)
					{
						request[event_wait[i].data.fd].create_file(request[event_wait[i].data.fd].outputFile, request[event_wait[i].data.fd].header_request);
						request[event_wait[i].data.fd].check_create_file = 1;
						if (request[event_wait[i].data.fd].fir_body != "NULL")
						{
							request[event_wait[i].data.fd].outputFile << request[event_wait[i].data.fd].fir_body;
							request[event_wait[i].data.fd].fir_body = "NULL";
						}
						else
						{
							cout <<"****************************************\n";

							request[event_wait[i].data.fd].outputFile << request[event_wait[i].data.fd].read_request;
						}
					}
					else
					{
						if (request[event_wait[i].data.fd].fir_body != "NULL")
						{
							request[event_wait[i].data.fd].outputFile << request[event_wait[i].data.fd].fir_body;
							request[event_wait[i].data.fd].fir_body = "NULL";
						}
						else
						{
							request[event_wait[i].data.fd].outputFile << request[event_wait[i].data.fd].read_request;
		 
						}
							
					}
	 
				}
				else if (request[event_wait[i].data.fd].methode == "GET")
					std::cout << "dyalk\n";
				  request[event_wait[i].data.fd].read_request.erase();
				// cout << request[event_wait[i].data.fd].size_request << " " <<  request[event_wait[i].data.fd].size_read_request << endl;
				if ( request[event_wait[i].data.fd].size_request < request[event_wait[i].data.fd].size_read_request)
				{
					// std::cout << "lllllllllll\n";
					const char n[170] = "HTTP/1.1 200 ok\r\nContent-Type:  text/html\r\nContent-Lenght:19\r\n\r\n <html><head><title>Hello Page</title></head><body><h1>Hello, client!</h1></body></html>";
					send(event_wait[i].data.fd, n, 170, 0);
					close(event_wait[i].data.fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_wait[i].data.fd, &event_wait[i]);
					std::map<int, Request>::iterator it = request.find(event_wait[i].data.fd);
					if (it != request.end())
						request.erase(it);
				}
				// cout << buff <<endl;
			}
			}
		}
	}


multiplexing::~multiplexing()
{
}