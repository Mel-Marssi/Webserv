#include "multiplexing.hpp"
#include "request.hpp"

multiplexing::multiplexing(servers &config)
{
	int server_socket[config.size()];
	struct sockaddr_in adress;
	int wait_fd, set_socket = 1;
	int fd_client;
	int epoll_fd = epoll_create(1);
	if (epoll_fd < 0)
		throw(runtime_error("epoll_create() call failed!"));
	struct epoll_event event, event_wait[1024];

	for (int i = 0; i < config.size(); i++)
	{
		stringstream int_to_string;
		if ((server_socket[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			throw(runtime_error("socket() call failed!"));
		adress.sin_addr.s_addr = inet_addr(config[i].get_host().c_str());
		adress.sin_family = AF_INET;
		adress.sin_port = htons(config[i].get_port());
		int_to_string << config[i].get_port();
		server_book[server_socket[i]] = make_pair(int_to_string.str(), config[i].get_host());

		if (setsockopt(server_socket[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &set_socket, sizeof(set_socket)) < 0)
			throw(runtime_error("setsockopt() call failed!"));
		if (bind(server_socket[i], (const sockaddr *)&adress, sizeof(adress)) < 0)
			throw(runtime_error("bind() call failed!"));
		if (listen(server_socket[i], 0) < 0)
			throw(runtime_error("listen() call failed!"));
		event.data.fd = server_socket[i];
		event.events = EPOLLIN | EPOLLOUT;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket[i], &event) < 0)
			throw(runtime_error("epoll_ctl() call failed!"));
	}
	// Methode GET fill Content Type : =======================
	this->fill_content_type();
	int flg_remv = 0;
	//================================================
	for (;;)
	{
		wait_fd = epoll_wait(epoll_fd, event_wait, 1024, 0);
		
		for (int i = 0; i < wait_fd; i++)
		{
			if (event_wait[i].data.fd <= server_socket[config.size() - 1])
			{
				server_book[event_wait[i].data.fd];
				if ((fd_client = accept(event_wait[i].data.fd, NULL, NULL)) < 0)
				{
					continue;
				}
				event.data.fd = fd_client;
				event.events = EPOLLIN | EPOLLOUT;
				server_book[fd_client] = server_book[event_wait[i].data.fd];
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_client, &event);
				request.insert(std::make_pair(fd_client, Request(server_book, fd_client)));
				request[fd_client].startTime = clock();
				request[fd_client].fd_request = fd_client;
				//   cout << fd_client <<endl;

			}
			else
			{
				request[event_wait[i].data.fd].fill_status_code();
				signal(SIGPIPE, SIG_IGN);
				if (request[event_wait[i].data.fd].check_left_header == 0 && (event_wait[i].events & EPOLLIN))
				{
						char buff[1024];
						request[event_wait[i].data.fd].size = 0;
						request[event_wait[i].data.fd].size = read(event_wait[i].data.fd, buff, 1024);
						request[event_wait[i].data.fd].startTime = clock();
						request[event_wait[i].data.fd].size_read_request += request[event_wait[i].data.fd].size;
						request[event_wait[i].data.fd].read_request.append(buff, request[event_wait[i].data.fd].size);
						request[event_wait[i].data.fd].parce_request(request[event_wait[i].data.fd].read_request, event_wait[i], epoll_fd, config);
			 	 	 
					if (request[event_wait[i].data.fd].check_left_header == 0)
						continue;
					else if (request[event_wait[i].data.fd].check_left_header == 1)
					{
						if(request[event_wait[i].data.fd].methode == "POST" && request[event_wait[i].data.fd].Handle_error(epoll_fd, config, event_wait[i]) == 1 )
						{

						}
						else if (  request[event_wait[i].data.fd].methode == "POST" && request[event_wait[i].data.fd].size_request == 0  && request[event_wait[i].data.fd].fir_body != "NULL")
				 		{
							request[event_wait[i].data.fd].error_page(event_wait[i], epoll_fd, "400", config);
							// close(event_wait[i].data.fd);
							epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_wait[i].data.fd, &event_wait[i]);

							flg_remv = 1;
							continue;
				 		}
					}
				}
				else if (request[event_wait[i].data.fd].check_left_header == 0 && !(event_wait[i].events & EPOLLIN) && !(request[event_wait[i].data.fd].size_request <= request[event_wait[i].data.fd].size_read_request || request[event_wait[i].data.fd].finir == 1 || request[event_wait[i].data.fd].err == 1))
				{
        			request[event_wait[i].data.fd].error_page(event_wait[i], epoll_fd, "400", config);
					// close(event_wait[i].data.fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_wait[i].data.fd, &event_wait[i]);
	 
					flg_remv = 1;
				}


				if (request[event_wait[i].data.fd].methode == "POST" && (event_wait[i].events & EPOLLIN))
				{
					request[fd_client].startTime = clock();
					request[event_wait[i].data.fd]._port = server_book[event_wait[i].data.fd].first;
					request[event_wait[i].data.fd]._host = server_book[event_wait[i].data.fd].second;

					request[event_wait[i].data.fd].post(event_wait[i].data.fd, config, event_wait[i]);
					request[event_wait[i].data.fd].check_read_get = 1;
				}
				else if ((request[event_wait[i].data.fd].methode == "GET") && (request[event_wait[i].data.fd].fin_or_still == Still))
				{
					request[fd_client].startTime = clock();

					request[event_wait[i].data.fd]._port = server_book[event_wait[i].data.fd].first;
					request[event_wait[i].data.fd]._host = server_book[event_wait[i].data.fd].second;
					request[event_wait[i].data.fd].event_fd = event_wait[i].data.fd;

					// request[event_wait[i].data.fd].index_serv = get_right_index(config.server, atoi(request[event_wait[i].data.fd]._port.c_str()), request[event_wait[i].data.fd]._host, config.get_server_name(atoi(request[event_wait[i].data.fd]._port.c_str())));
					// cout << "index :::::::::::::      " << request[event_wait[i].data.fd].index_serv << endl;
					if (request[event_wait[i].data.fd].check_req == 0 && request[event_wait[i].data.fd].read_get == 1)
					{
						request[event_wait[i].data.fd].Generate_req_first(event_wait[i], config, epoll_fd, cont_type);
						// update changes
						request[event_wait[i].data.fd].cgi_handle_get(epoll_fd, event_wait[i], config);
						//end update changes
					}
					else if (request[event_wait[i].data.fd].check_req == 1)
						request[event_wait[i].data.fd].Generate_req_second(event_wait[i], epoll_fd);
					if (request[event_wait[i].data.fd].fin_or_still == finish)
						flg_remv = 1;

					request[event_wait[i].data.fd].read_get = 1;
				}
				else if (request[event_wait[i].data.fd].methode == "DELETE")
				{
					request[event_wait[i].data.fd].epoll_fd_tmp = epoll_fd;
					request[event_wait[i].data.fd].Delete_Function(event_wait[i], config, epoll_fd, cont_type);
				}
				if (request[event_wait[i].data.fd].methode == "NONE")
				{
					request[event_wait[i].data.fd].error_page(event_wait[i], epoll_fd, "400", config);
					flg_remv = 1;
				}
				//   cout << request[event_wait[i].data.fd].err << " " << request[event_wait[i].data.fd].methode << " " << (event_wait[i].events & EPOLLOUT )<< endl;
				if ((request[event_wait[i].data.fd].methode == "POST" || request[event_wait[i].data.fd].methode == "GET" ) &&  (event_wait[i].events & EPOLLOUT ) && ((request[event_wait[i].data.fd].size_request <= request[event_wait[i].data.fd].size_read_request ) || request[event_wait[i].data.fd].finir == 1 || request[event_wait[i].data.fd].err == 1))// ||request[event_wait[i].data.fd].size_request < request[event_wait[i].data.fd].size_read_request || request[event_wait[i].data.fd].finir == 1 || request[event_wait[i].data.fd].err == 1))
				{
					// cout << request[event_wait[i].data.fd].finir  << " " << request[event_wait[i].data.fd].err<<endl ;
					// cout << "******\n";
					request[fd_client].startTime = clock();
					if (request[event_wait[i].data.fd].status_pro != "NULL")
					{
						flg_remv = 1;
						request[event_wait[i].data.fd].error_page(event, event_wait[i].data.fd, request[event_wait[i].data.fd].status_pro , config);

					}
					else if (request[event_wait[i].data.fd].methode == "POST")
					{
						const char n[170] = "HTTP/1.1 201 created\r\nContent-Type:  text/html\r\nContent-Lenght:19\r\n\r\n <html><head><title>Hello Page</title></head><body><h1>Hello, client!</h1></body></html>";
						send(event_wait[i].data.fd, n, 170, 0);
						flg_remv = 1;
					}
				}
				if (flg_remv == 1)
				{
					// cout << "hnaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n";
					close(event_wait[i].data.fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_wait[i].data.fd, &event_wait[i]);
			 
					std::map<int, Request>::iterator it = request.find(event_wait[i].data.fd);
					if (it != request.end())
						request.erase(it);
					request[event_wait[i].data.fd].outputFile.close();
					flg_remv = 0;
				}
 
			}
		}

		// int y = server_socket[config.size() - 1]+1;
		// std::map<int, Request>::iterator it = request.find(y);
		// if (it != request.end())
		// {
			// for (size_t i = 0; i < request.size(); i++)
			// {
			// 	if (event_wait[i].data.fd > server_socket[config.size() - 1] && !(event_wait[i].events & EPOLLIN ))
			// 	{
			// 		clock_t endTime = clock();
			// 			double resultInSeconds = static_cast<double>(endTime - request[event_wait[i].data.fd].startTime) / CLOCKS_PER_SEC;
			// 			if (resultInSeconds >= 10 && request[event_wait[i].data.fd].status_pro != "504")
			// 			{
			// 				cout << "frefrerefe\n";
			// 		  		cout << fd_client <<endl;
			// 				const char n[170] = "HTTP/1.1 504 created\r\nContent-Type:  text/html\r\nContent-Lenght:19\r\n\r\n <html><head><title>timeout</title></head><body><h1>Hello, client!</h1></body></html>";
			// 			send(event_wait[i].data.fd, n, 170, 0);
			// 			 close(event_wait[i].data.fd);
			// 			std::map<int, Request>::iterator it = request.find((const int)event_wait[i].data.fd);
			// 			if (it != request.end())
			// 				request.erase((const int)event_wait[i].data.fd);
		
			// 			}

			// 	}
			// }
			
	}
}

multiplexing::~multiplexing()
{
}