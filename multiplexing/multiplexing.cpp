
#include "multiplexing.hpp"
#include "request.hpp"
#include "../cgi-bin/cgi_handler.hpp"

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
		event.events = EPOLLIN | EPOLLOUT | EPOLLERR;
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
			int event_fd = event_wait[i].data.fd;
			if (event_fd <= server_socket[config.size() - 1])
			{
				server_book[event_fd];
				if ((fd_client = accept(event_fd, NULL, NULL)) < 0)
				{
					continue;
				}
				event.data.fd = fd_client;
				event.events = EPOLLIN | EPOLLOUT | EPOLLERR;
				server_book[fd_client] = server_book[event_fd];
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_client, &event);
				request.insert(std::make_pair(fd_client, Request(server_book, fd_client)));
				request[fd_client].startTime = clock();
				request[fd_client].fd_request = fd_client;
			}
			else
			{
				request[event_fd].fill_status_code();
				signal(SIGPIPE, SIG_IGN);
				if (request[event_fd].check_left_header == 0 && (event_wait[i].events & EPOLLIN))
				{
					char buff[1024];
					request[event_fd].size = 0;
					request[event_fd].size = read(event_fd, buff, 1024);
					request[event_fd].startTime = clock();
					request[event_fd].size_read_request += request[event_fd].size;
					if (request[event_fd].size != -1)
					{
						request[event_fd].read_request.append(buff, request[event_fd].size);
						request[event_fd].parce_request(request[event_fd].read_request, event_wait[i], epoll_fd, config);
					}
					// cout << request[event_fd].read_request << endl;
					if (request[event_fd].check_left_header == 0)
					{
						continue;
					}
					else if (request[event_fd].check_left_header == 1)
					{
						if (request[event_fd].methode == "POST" && request[event_fd].Handle_error(epoll_fd, config, event_wait[i]) == 1)
						{
						}
					}
				}

				if (request[event_fd].methode == "POST" && (event_wait[i].events & EPOLLIN))
				{

					request[fd_client].startTime = clock();
					request[event_fd]._port = server_book[event_fd].first;
					request[event_fd]._host = server_book[event_fd].second;

					request[event_fd].post(event_fd, config, event_wait[i]);
					request[event_fd].check_read_get = 1;

					// request[event_fd].read_request.clear();
				}
				else if ((request[event_fd].methode == "GET") && (request[event_fd].fin_or_still == Still) && request[event_fd].check_left_header == 1)
				{
					request[fd_client].startTime = clock();
					request[event_fd]._port = server_book[event_fd].first;
					request[event_fd]._host = server_book[event_fd].second;
					request[event_fd].event_fd = event_fd;

					request[event_fd].Get_methode(config, event_wait[i], cont_type);
					if (request[event_fd].fin_or_still == finish)
					{
						flg_remv = 1;
					}
				}
				else if (request[event_fd].methode == "DELETE")
				{
					request[fd_client].startTime = clock();
					request[event_fd].epoll_fd_tmp = epoll_fd;
					request[event_fd].Delete_Function(event_wait[i], config, epoll_fd, cont_type);
					request[event_fd].response_for_delete(event_wait[i]);
					flg_remv = 1;
				}
				if (request[event_fd].methode == "NONE" || request[event_fd].methode == "HEAD")
				{
					request[event_fd].error_page(event_wait[i], "501", config);
					flg_remv = 1;
				}

				if (request[fd_client].cgi_post == true)
				{
					cout << request[event_fd].path_post << endl;
					cgi_exec_post(request[event_fd], config[get_right_index(config.server, atoi (request[event_fd]._port.c_str()), request[event_fd]._host, request[event_fd].Path)]);
					send(event_fd, request[event_fd].resp_post().c_str(), 862, 0);
					close(event_fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);

					std::map<int, Request>::iterator it = request.find(event_fd);
					if (it != request.end())
						request.erase(it);
					request[event_fd].outputFile.close();
					flg_remv = 0;
				}

				if ((request[event_fd].methode == "POST" || request[event_fd].methode == "GET") && (event_wait[i].events & EPOLLOUT) && request[event_fd].check_left_header == 1 && ((request[event_fd].size_request <= request[event_fd].size_read_request) || request[event_fd].finir == 1 || request[event_fd].err == 1)) // ||request[event_fd].size_request < request[event_fd].size_read_request || request[event_fd].finir == 1 || request[event_fd].err == 1))
				{
					// if (request[event_fd].size_chuked == )
					request[fd_client].startTime = clock();
					if (request[event_fd].status_pro != "NULL")
					{
						flg_remv = 1;
						request[event_fd].error_page(event, request[event_fd].status_pro, config);
					}
					else if (request[event_fd].methode == "POST")
					{
						if (request[event_fd].path_post == "NULL")
							request[event_fd].error_page(event_wait[i], "400", config);
						else
						{
							if (request[event_fd].Path == "/cgi-bin")
							{
								request[event_fd].finir = 0;
								request[event_fd].size_read_request = -1;
								request[event_fd].cgi_post = true;
								continue;
							}
							send(event_fd, request[event_fd].resp_post().c_str(), 862, 0);
						}
						close(event_fd);
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);

						std::map<int, Request>::iterator it = request.find(event_fd);
						if (it != request.end())
							request.erase(it);
						request[event_fd].outputFile.close();
						flg_remv = 0;
					}
				}
			}
			if (flg_remv == 1)
			{
				cout << request[event_fd].zompie << endl;
				close(event_fd);
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);
				std::map<int, Request>::iterator it = request.find(event_fd);
				if (it != request.end())
					request.erase(it);
				request[event_fd].outputFile.close();
				flg_remv = 0;
			}
			else if (request[event_fd].startTime > 0)
			{
				// cout <<request[event_fd].startTime  << " " << event_fd << " " << event_wait[i].data.fd << endl;
				clock_t endTime = clock();

				if ((endTime - request[event_fd].startTime) / CLOCKS_PER_SEC >= 8)
				{
					request[event_fd].error_page(event_wait[i], "504", config);
					close(request[event_fd].fd_request);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);
					request.erase(event_fd);
				}
			}
		}
	}
}

multiplexing::~multiplexing()
{
}
