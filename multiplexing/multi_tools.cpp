#include "multiplexing.hpp"

int multiplexing::read_request(int event_fd, servers &config, int i)
{
	char buff[1024];
	request[event_fd].size = 0;
	request[event_fd].size = read(event_fd, buff, 1024);
	if (request[event_fd].size <= 0)
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
		return 1;
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
			else
			{
				request[event_fd].index_serv = get_right_index(config.server, atoi(server_book[event_fd].first.c_str()), server_book[event_fd].second, request[event_fd]._host);
			}
			if (request[event_fd].index_serv == -1)
			{
				request[event_fd].index_serv = 0;
				request[event_fd].error_page(event_wait[i], "400", config);
				if (request.find(event_fd) != request.end())
					request.erase(event_fd);
				close(event_fd);
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_wait[i].data.fd, &event_wait[i]);
				return 1;
			}
		}
		else
		{
			request[event_fd].status_pro = "400";
			request[event_fd].index_serv = 0;
		}
		if (request[event_fd].methode == "POST" && request[event_fd].Handle_error(epoll_fd, config, event_wait[i]) == 1)
		{
		}
	}
	return 0;
}

void multiplexing::error_epoll(int event_fd, int i)
{
	if (request[event_fd].pid != 0)
	{
		kill(request[event_fd].pid, SIGKILL);
		waitpid(request[event_fd].pid, NULL, 0);
		remove(request[event_fd].cgi_file.c_str());
	}
	map<int, Request>::iterator it = request.find(event_fd);
	if (it != request.end())
		request.erase(it);
	request[event_fd].outputFile.close();
	close(event_fd);
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);
}

void multiplexing::delete_method(int event_fd, servers &config, int i)
{
	gettimeofday(&request[event_fd].startTime, NULL);
	request[event_fd].epoll_fd_tmp = epoll_fd;
	request[event_fd].Delete_Function(event_wait[i], config);
	request[event_fd].response_for_delete(event_wait[i]);
	flg_remv = 1;
}

int multiplexing::send_response(int event_fd, servers &config, int i)
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
		{
			request[event_fd].error_page(event_wait[i], "400", config);
		}
		else
		{
			if (request[event_fd].full_Path != "/cgi-bin" && request[event_fd].full_Path != "/cgi-bin/upload.php" && request[event_fd].Path == "/cgi-bin" && request[event_fd].cgi_post == false)
			{
				request[event_fd].finir = 0;
				request[event_fd].size_read_request = -1;
				request[event_fd].cgi_post = true;
				request[event_fd].outputFile.close();
				return 1;
			}
			if (send(event_fd, request[event_fd].resp_post().c_str(), 862, 0) <= 0)
				request[event_fd].error_page(event_wait[i], "500", config);
		}
		if (request[event_fd].pid != 0)
		{
			kill(request[event_fd].pid, SIGKILL);
			waitpid(request[event_fd].pid, NULL, 0);
			remove(request[event_fd].cgi_file.c_str());
		}
		request[event_fd].outputFile.close();
		map<int, Request>::iterator it = request.find(event_fd);
		if (it != request.end())
			request.erase(it);
		close(event_fd);
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);
		flg_remv = 0;
		return 1;
	}
	if (flg_remv == 1)
	{
		if (request[event_fd].pid != 0)
		{
			kill(request[event_fd].pid, SIGKILL);
			waitpid(request[event_fd].pid, NULL, 0);
			remove(request[event_fd].cgi_file.c_str());
		}
		map<int, Request>::iterator it = request.find(event_fd);
		if (it != request.end())
			request.erase(it);
		close(event_fd);
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);
		request[event_fd].outputFile.close();
		flg_remv = 0;
	}
	return 0;
}

int multiplexing::get_methode(int event_fd, servers &config, int i)
{
	gettimeofday(&request[event_fd].startTime, NULL);
	request[event_fd]._port = server_book[event_fd].first;
	request[event_fd].Get_methode(config, event_wait[i], cont_type);
	if ((request[event_fd].status_pro == "504" || request[event_fd].status_pro == "500") && !request[event_fd].cgi_file.empty())
	{
		request[event_fd].error_page(event_wait[i], request[event_fd].status_pro, config);
		if (request[event_fd].pid != 0)
		{
			kill(request[event_fd].pid, SIGKILL);
			waitpid(request[event_fd].pid, NULL, 0);
			remove(request[event_fd].cgi_file.c_str());
		}
		map<int, Request>::iterator it = request.find(event_fd);
		if (it != request.end())
			request.erase(it);
		close(event_fd);
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);
		return 1;
	}
	if (request[event_fd].fin_or_still == finish)
	{
		flg_remv = 1;
	}
	return 0;
}

void multiplexing::time_out_post(int event_fd, servers &config, int i)
{
	struct timeval end;
	gettimeofday(&end, NULL);
	double timeOut = static_cast<double>(((end.tv_sec) - (request[event_fd].startTime.tv_sec)));
	if (request[event_fd].timeOut == false && (timeOut >= 30) && request.find(event_fd) != request.end())
	{
		if (request[event_fd].status_pro != "NULL")
			request[event_fd].error_page(event_wait[i], request[event_fd].status_pro, config);
		else
			request[event_fd].error_page(event_wait[i], "504", config);
		if (request[event_fd].pid != 0)
		{
			kill(request[event_fd].pid, SIGKILL);
			waitpid(request[event_fd].pid, NULL, 0);
			remove(request[event_fd].cgi_file.c_str());
		}
		if (request.find(event_fd) != request.end())
			request.erase(event_fd);
		close(event_fd);
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);
	}
}

void multiplexing::post_boundry(int event_fd, servers &config, int i)
{
	gettimeofday(&request[event_fd].startTime, NULL);
	request[event_fd]._port = server_book[event_fd].first;
	request[event_fd]._host = server_book[event_fd].second;
	request[event_fd].post(event_fd, config, event_wait[i]);
	request[event_fd].check_read_get = 1;
}

void multiplexing::cgi_post(int event_fd, servers &config, int i)
{
	int status;
	struct timeval end;
	char buff[1024];
	string response;
	memset(buff, 0, 1024);
	if (request[event_fd].pid == 0)
		execute_cgi(request[event_fd], config[i]);
	try
	{
		if ((waitpid(request[event_fd].pid, &status, WNOHANG)) == 0)
		{
			gettimeofday(&end, NULL);
			double timeOut = static_cast<double>(((end.tv_sec + end.tv_usec / 1000000) - (request[event_fd].start_cgi.tv_sec + request[event_fd].start_cgi.tv_usec / 1000000)));
			if (timeOut >= 30)
			{
				if (request[event_fd].pid != 0)
				{
					kill(request[event_fd].pid, SIGKILL);
					waitpid(request[event_fd].pid, &status, 0);
					remove(request[event_fd].cgi_file.c_str());
				}
				request[event_fd].error_page(event_wait[i], "504", config);
				throw "504";
			}
			else if (timeOut != 0)
				request[event_fd].timeOut = true;
		}
		else if (((WIFEXITED(status) && WEXITSTATUS(status) != 0) || (WIFSIGNALED(status) && request[event_fd].im_reading0 == false)) && request[event_fd].pid != 0)
		{
			request[event_fd].error_page(event_wait[i], "500", config);
			throw "500";
		}
		else
		{
			if (request[event_fd].op_cgi.is_open() == false)
				request[event_fd].op_cgi.open(request[event_fd].cgi_file.c_str());
			if (event_wait[i].events & EPOLLOUT)
			{
				if (request[event_fd].add_header_response == 0)
				{
					request[event_fd].add_header_response = 1;
					response = "HTTP/1.1 200 OK\r\n";
				}
				request[event_fd].op_cgi.read(buff, 1024);
				if (request[event_fd].op_cgi.bad() == true)
					throw "500";
				response += buff;
				if (send(event_fd, response.c_str(), response.length(), 0) <= 0)
					throw "500";
				if (request[event_fd].op_cgi.eof() == true)
					throw "";
				else
				{
					request[event_fd].cgi_post = true;
					request[event_fd].im_reading0 = true;
				}
			}
		}
	}
	catch (const char *e)
	{
		if (strcmp(e, "") != 0)
			request[event_fd].error_page(event_wait[i], e, config);
		if (request[event_fd].pid != 0)
		{
			kill(request[event_fd].pid, SIGKILL);
			waitpid(request[event_fd].pid, &status, 0);
			remove(request[event_fd].cgi_file.c_str());
		}
		remove(request[event_fd].path_post.c_str());
		map<int, Request>::iterator it = request.find(event_fd);
		if (it != request.end())
			request.erase(it);
		close(event_fd);
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event_fd, &event_wait[i]);
	}
}

int multiplexing::accept_client(int event_fd)
{
	server_book[event_fd];
	if ((fd_client = accept(event_fd, NULL, NULL)) < 0)
		return 1;
	event.data.fd = fd_client;
	event.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
	server_book[fd_client] = server_book[event_fd];
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd_client, &event);
	request.insert(make_pair(fd_client, Request(server_book, fd_client)));
	gettimeofday(&request[fd_client].startTime, NULL);
	request[fd_client].fd_request = fd_client;
	return 0;
}