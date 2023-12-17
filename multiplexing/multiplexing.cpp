#include "multiplexing.hpp"

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
			}
			else
			{
				// for(int i = 0; i < ()vec_client.size(); i++)
				{
					char buff[1000];
					read(event_wait[i].data.fd, buff, 1000);
					const char n[170] = "HTTP/1.1 200 ok\r\nContent-Type:  text/html\r\nContent-Lenght:19\r\n\r\n <html><head><title>Hello Page</title></head><body><h1>Hello, client!</h1></body></html>";
					send(event_wait[i].data.fd, n, 170, 0);
					close(event_wait[i].data.fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_wait[i].data.fd, &event_wait[i]);
					cout << buff <<endl;
				}
				
			}
		}
	}
}

multiplexing::~multiplexing()
{
}