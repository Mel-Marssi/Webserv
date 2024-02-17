
#include "multiplexing.hpp"
#include "request.hpp"
#include "../cgi-bin/cgi_handler.hpp"

multiplexing::multiplexing(servers &config)
{
	server_socket = new int[config.size()];
	set_socket = 1;
	flg_remv = 0;
	epoll_fd = epoll_create(1);
	if (epoll_fd < 0)
	{
		delete[] server_socket;
		throw(runtime_error("epoll_create() call failed!"));
	}
}

multiplexing::~multiplexing()
{
}

unsigned long ft_inet_addr(string str)
{
	unsigned long addr = 0;
	int i = 0;
	long long byteValue[4];
	int shift = 24;
	stringstream ss(str);
	string byteStr;

	while (getline(ss, byteStr, '.'))
	{
		if (i >= 4)
			throw(runtime_error("Invalid IP address format not IPv4 format"));
		byteValue[i] = atoi(byteStr.c_str());
		if (byteValue[i] < 0 || byteValue[i] > 255)
			throw(runtime_error("Invalid IP address format"));
		i++;
	}
	i = 3;
	while (shift >= 0 && i >= 0)
	{
		addr += byteValue[i] << shift;
		shift -= 8;
		i--;
	}
	return addr;
}

void multiplexing::setup_server_socket(servers &config)
{
	for (int i = 0; i < config.size(); i++)
	{
		stringstream int_to_string;
		if ((server_socket[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			throw(runtime_error("socket() call failed!"));
		adress.sin_addr.s_addr = ft_inet_addr(config[i].get_host());
		adress.sin_family = AF_INET;
		adress.sin_port = htons(config[i].get_port());
		int_to_string << config[i].get_port();
		server_book[server_socket[i]] = make_pair(int_to_string.str(), config[i].get_host());

		if (setsockopt(server_socket[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &set_socket, sizeof(set_socket)) < 0)
		{
			for (int j = 0; j <= i; j++)
			{
				close(epoll_fd);
				close(server_socket[j]);
				delete[] server_socket;
			}
			throw(runtime_error("setsockopt() call failed!"));
		}
		if (bind(server_socket[i], (const sockaddr *)&adress, sizeof(adress)) < 0)
		{
			for (int j = 0; j <= i; j++)
			{
				close(epoll_fd);
				close(server_socket[j]);
				delete[] server_socket;
			}
			throw(runtime_error("bind() call failed!"));
		}
		if (listen(server_socket[i], 1024) < 0)
		{
			for (int j = 0; j <= i; j++)
			{
				close(epoll_fd);
				close(server_socket[j]);
				delete[] server_socket;
			}
			throw(runtime_error("listen() call failed!"));
		}
		event.data.fd = server_socket[i];
		event.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket[i], &event) < 0)
		{
			for (int j = 0; j < i; j++)
			{
				close(epoll_fd);
				close(server_socket[j]);
				delete[] server_socket;
			}
			throw(runtime_error("epoll_ctl() call failed!"));
		}
	}
	fill_content_type();
}

void multiplexing::run(servers &config)
{
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
					continue;
				event.data.fd = fd_client;
				event.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
				server_book[fd_client] = server_book[event_fd];
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_client, &event);
				request.insert(make_pair(fd_client, Request(server_book, fd_client)));
				gettimeofday(&request[fd_client].startTime, NULL);
				request[fd_client].fd_request = fd_client;
			}
			else
			{
				request[event_fd].fill_status_code();
				signal(SIGPIPE, SIG_IGN);
				if (event_wait[i].events & EPOLLRDHUP || event_wait[i].events & EPOLLERR || event_wait[i].events & EPOLLHUP)
				{
					if (request[event_fd].pid != 0)
					{
						kill(request[event_fd].pid, SIGKILL);
						waitpid(request[event_fd].pid, NULL, 0);
						remove(request[event_fd].cgi_file.c_str());
					}
					close(event_fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);
					map<int, Request>::iterator it = request.find(event_fd);
					if (it != request.end())
						request.erase(it);
					request[event_fd].outputFile.close();
					continue;
				}

				if (request[event_fd].check_left_header == 0 && (event_wait[i].events & EPOLLIN))
				{
					char buff[1024];
					request[event_fd].size = 0;
					request[event_fd].size = read(event_fd, buff, 1024);
					if (request[event_fd].size < 0)
						request[event_fd].status_pro = "500";
					gettimeofday(&request[event_fd].startTime, NULL);
					request[event_fd].size_read_request += request[event_fd].size;

					if (request[event_fd].size != -1)
					{
						request[event_fd].read_request.append(buff, request[event_fd].size);
						request[event_fd].parce_request(request[event_fd].read_request, event_wait[i], epoll_fd, config);
					}
					if (request[event_fd].check_left_header == 0)
					{
						continue;
					}
					else if (request[event_fd].check_left_header == 1)
					{
						map<string, string>::iterator it = request[event_fd].header_request.find("Host");
						if (it != request[event_fd].header_request.end())
						{
							request[event_fd]._host = request[event_fd].header_request["Host"];
							size_t pos_host = 0;
							pos_host = request[event_fd]._host.find(":");
							if (pos_host != string::npos)
							{
								request[event_fd]._host.erase(pos_host, request[event_fd]._host.length());
								request[event_fd].index_serv = get_right_index(config.server, atoi(server_book[event_fd].first.c_str()), server_book[event_fd].second, request[event_fd]._host);
							}
						}
						else
							request[event_fd].status_pro = "400";

						if (request[event_fd].methode == "POST" && request[event_fd].Handle_error(epoll_fd, config, event_wait[i]) == 1)
						{
						}
					}
				}

				if ((request[event_fd].methode == "POST" && (event_wait[i].events & EPOLLIN)) || request[event_fd].fake_bondary != "NULL")
				{

					gettimeofday(&request[event_fd].startTime, NULL);
					request[event_fd]._port = server_book[event_fd].first;
					request[event_fd]._host = server_book[event_fd].second;

					request[event_fd].post(event_fd, config, event_wait[i]);
					request[event_fd].check_read_get = 1;
				}
				else if ((request[event_fd].methode == "GET") && (request[event_fd].fin_or_still == Still) && request[event_fd].check_left_header == 1)
				{
					gettimeofday(&request[event_fd].startTime, NULL);
					request[event_fd]._port = server_book[event_fd].first;
					request[event_fd].Get_methode(config, event_wait[i], cont_type);
					if (request[event_fd].fin_or_still == finish)
						flg_remv = 1;
				}
				else if (request[event_fd].methode == "DELETE")
				{
					gettimeofday(&request[event_fd].startTime, NULL);
					request[event_fd].epoll_fd_tmp = epoll_fd;
					request[event_fd].Delete_Function(event_wait[i], config);
					request[event_fd].response_for_delete(event_wait[i]);
					flg_remv = 1;
				}

				if (request[event_fd].methode == "NONE" || request[event_fd].methode == "HEAD")
				{
					request[event_fd].error_page(event_wait[i], "501", config);
					flg_remv = 1;
				}

				if (request[event_fd].cgi_post == true)
				{
					string head;
					string tmp_path = "/" + request[event_fd].path_post;
					size_t length;

					head += "HTTP/1.1 ";
					head += "301 Moved Permanently\r\nLocation: ";
					i = request[event_fd].path_post.find("/", 1);
					head += tmp_path;
					head += "\r\n\r\n";
					length = head.length();
					send(event.data.fd, head.c_str(), length, 0);
					if (!request[event_fd].cgi_file.empty())
							remove(request[event_fd].cgi_file.c_str());
						close(event_fd);
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);
						map<int, Request>::iterator it = request.find(event_fd);
						if (it != request.end())
							request.erase(it);
						request[event_fd].outputFile.close();
						continue;
				}

				if ((request[event_fd].methode == "POST" || request[event_fd].methode == "GET") && (event_wait[i].events & EPOLLOUT) && request[event_fd].check_left_header == 1 && ((request[event_fd].type != "chunked" && request[event_fd].size_request <= request[event_fd].size_read_request && request[event_fd].size_read_request > 0) || request[event_fd].finir == 1 || request[event_fd].err == 1))
				{
					gettimeofday(&request[event_fd].startTime, NULL);
					if (request[event_fd].status_pro != "NULL")
					{
						flg_remv = 1;
						request[event_fd].error_page(event_wait[i], request[event_fd].status_pro, config);
					}
					else if (request[event_fd].methode == "POST")
					{
						if (request[event_fd].path_post == "NULL")
							request[event_fd].error_page(event_wait[i], "400", config);
						else
						{
							if (request[event_fd].Path == "/cgi-bin" && request[event_fd].cgi_post == false)
							{
								request[event_fd].finir = 0;
								request[event_fd].size_read_request = -1;
								request[event_fd].cgi_post = true;
								continue;
							}
							if (send(event_fd, request[event_fd].resp_post().c_str(), 862, 0) < 0)
								request[event_fd].error_page(event_wait[i], "500", config);
						}
						close(event_fd);
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);

						std::map<int, Request>::iterator it = request.find(event_fd);
						if (it != request.end())
							request.erase(it);
						request[event_fd].outputFile.close();
						flg_remv = 0;
						continue;
					}

					if (flg_remv == 1)
					{
						if (!request[event_fd].cgi_file.empty())
							remove(request[event_fd].cgi_file.c_str());
						close(event_fd);
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);
						map<int, Request>::iterator it = request.find(event_fd);
						if (it != request.end())
							request.erase(it);
						request[event_fd].outputFile.close();
						flg_remv = 0;
					}
				}
				if (event_fd > server_socket[config.size() - 1] && request[event_fd].startTime.tv_sec > 0)
				{
					struct timeval end;
					gettimeofday(&end, NULL);
					size_t timeOut = static_cast<size_t>(((end.tv_sec) - (request[event_fd].startTime.tv_sec)));
					if ((timeOut >= 30))
					{
						if (request[event_fd].status_pro != "NULL")
                            request[event_fd].error_page(event_wait[i], request[event_fd].status_pro, config);
                        else
							request[event_fd].error_page(event_wait[i], "504", config);
						close(event_fd);
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);
						request.erase(event_fd);
					}
				}
			}
		}
	}
}
