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
		{
			for (int j = 0; j <= i; j++)
			{
				close(epoll_fd);
				close(server_socket[j]);
				delete[] server_socket;
			}
			throw(runtime_error("socket() call failed!"));
		}
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
		if (listen(server_socket[i], -1) < 0)
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
			if (event_fd <= 0)
			{
				break;
			}
			if (event_fd <= server_socket[config.size() - 1])
			{
				if (accept_client(event_fd) == 1)
					continue;
			}
			else
			{
				request[event_fd].fill_status_code();
				signal(SIGPIPE, SIG_IGN);
				if (event_wait[i].events & EPOLLRDHUP || event_wait[i].events & EPOLLERR || event_wait[i].events & EPOLLHUP)
				{
					error_epoll(event_fd, i);
					continue;
				}
				if (request[event_fd].check_left_header == 0 && (event_wait[i].events & EPOLLIN))
				{
					if (read_request(event_fd, config, i) == 1)
						continue;
				}
				if ((request[event_fd].methode == "POST" && (event_wait[i].events & EPOLLIN) && !(request[event_fd].cgi_post == true)) || request[event_fd].fake_bondary != "NULL")
				{
					post_boundry(event_fd, config, i);
				}
				else if ((request[event_fd].methode == "GET") && (request[event_fd].fin_or_still == Still) && request[event_fd].check_left_header == 1)
				{
					if (get_methode(event_fd, config, i) == 1)
						continue;
				}
				else if (request[event_fd].methode == "DELETE")
				{
					delete_method(event_fd, config, i);
					map<int, Request>::iterator it = request.find(event_fd);
					if (it != request.end())
						request.erase(it);
					request[event_fd].outputFile.close();
					close(event_fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);
					continue;
				}

				if (request[event_fd].methode == "NONE" || request[event_fd].methode == "HEAD")
				{
					request[event_fd].error_page(event_wait[i], "501", config);
					map<int, Request>::iterator it = request.find(event_fd);
					if (it != request.end())
						request.erase(it);
					request[event_fd].outputFile.close();
					close(event_fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);
					continue;
				}
				if (request[event_fd].cgi_post == true)
				{
					cgi_post(event_fd, config, i);
					continue;
				}
				if ((request[event_fd].methode == "POST" || request[event_fd].methode == "GET") && (event_wait[i].events & EPOLLOUT) && request[event_fd].check_left_header == 1 && ((request[event_fd].type != "chunked" && request[event_fd].size_request <= request[event_fd].size_read_request && request[event_fd].size_read_request > 0) || request[event_fd].finir == 1 || request[event_fd].err == 1))
				{
					if (send_response(event_fd, config, i) == 1)
						continue;
				}
				if (event_fd > server_socket[config.size() - 1] && request[event_fd].startTime.tv_sec > 0)
				{
					time_out_post(event_fd, config, i);
				}
			}
		}
	}
}