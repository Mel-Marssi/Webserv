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
					continue;
				event.data.fd = fd_client;
				event.events = EPOLLIN | EPOLLOUT;
				server_book[fd_client] = server_book[event_wait[i].data.fd];
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_client, &event);
				request.insert(std::make_pair(fd_client, Request(server_book, fd_client)));
			}
			else
			{
				request[event_wait[i].data.fd].fill_status_code();

				if (request[event_wait[i].data.fd].check_left_header == 0)
				{
					char buff[1024];
					request[event_wait[i].data.fd].size = 0;
					request[event_wait[i].data.fd].size = read(event_wait[i].data.fd, buff, 1024);
					request[event_wait[i].data.fd].size_read_request += request[event_wait[i].data.fd].size;
					request[event_wait[i].data.fd].read_request.append(buff, request[event_wait[i].data.fd].size);
					request[event_wait[i].data.fd].parce_request(request[event_wait[i].data.fd].read_request, event_wait[i], epoll_fd, config);
					// cout << request[event_wait[i].data.fd].read_request << endl;
				}

				if (request[event_wait[i].data.fd].methode == "POST")
				{

					request[event_wait[i].data.fd]._port = server_book[event_wait[i].data.fd].first;
					request[event_wait[i].data.fd]._host = server_book[event_wait[i].data.fd].second;

					request[event_wait[i].data.fd].post(event_wait[i].data.fd, config, event_wait[i]);
				}
				else if (request[event_wait[i].data.fd].methode == "GET" && request[event_wait[i].data.fd].fin_or_still == Still)
				{
					request[event_wait[i].data.fd]._port = server_book[event_wait[i].data.fd].first;
					request[event_wait[i].data.fd]._host = server_book[event_wait[i].data.fd].second;
					request[event_wait[i].data.fd].event_fd = event_wait[i].data.fd;
					// cout << "----->" << request[event_wait[i].data.fd]._port << " " << request[event_wait[i].data.fd]._host <<endl;
					if (request[event_wait[i].data.fd].check_req == 0)
					{
						// request[event_wait[i].data.fd].fill_status_code();
						request[event_wait[i].data.fd].Generate_req_first(event_wait[i], config, epoll_fd, cont_type);
						// update changes
						if (request[event_wait[i].data.fd].Path.find("cgi") != std::string::npos)
						{
							int out;
							//  start = clock();
							if ((out = waitpid(request[event_wait[i].data.fd].pid, NULL, WNOHANG)) == 0)
							{
								request[event_wait[i].data.fd].get_to_cgi = true;
								double tmp = (clock() - request[event_wait[i].data.fd].start) / CLOCKS_PER_SEC;
								if (tmp >= 5)
								{
									cout << "kill : " << tmp << endl;
									kill(request[event_wait[i].data.fd].pid, SIGKILL);
									request[event_wait[i].data.fd].error_page(event_wait[i], epoll_fd, "504", config);
								}
							}
							else
							{
								request[event_wait[i].data.fd].get_to_cgi = false;
								if (request[event_wait[i].data.fd].op_cgi.is_open())
								{

									request[event_wait[i].data.fd].read_for_send(cont_type, 1);
									if (request[event_wait[i].data.fd].op_cgi.eof())
										request[event_wait[i].data.fd].end_of_file(event, epoll_fd);
								}
							}
						}
						//end update changes
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
				else if (request[event_wait[i].data.fd].methode == "DELETE")
				{
					request[event_wait[i].data.fd].epoll_fd_tmp = epoll_fd;
					request[event_wait[i].data.fd].Delete_Function(event_wait[i], config, epoll_fd, cont_type);
				}
				if (request[event_wait[i].data.fd].methode == "NONE")
				{
					request[event_wait[i].data.fd].error_page(event_wait[i], epoll_fd, "400", config);
					std::map<int, Request>::iterator it = request.find(event_wait[i].data.fd);
					if (it != request.end())
						request.erase(it);
					request[event_wait[i].data.fd].read_request = "";
				}
				if (request[event_wait[i].data.fd].methode == "POST" && (request[event_wait[i].data.fd].size_request < request[event_wait[i].data.fd].size_read_request || request[event_wait[i].data.fd].finir == 1 || request[event_wait[i].data.fd].err == 1))
				{
					if (request[event_wait[i].data.fd].err == 0)
					{
						const char n[170] = "HTTP/1.1 201 created\r\nContent-Type:  text/html\r\nContent-Lenght:19\r\n\r\n <html><head><title>Hello Page</title></head><body><h1>Hello, client!</h1></body></html>";
						send(event_wait[i].data.fd, n, 170, 0);
					}
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