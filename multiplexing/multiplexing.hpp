#pragma once

#include "request.hpp"
#include <arpa/inet.h>
#include <sys/epoll.h>

typedef vector<int>::iterator iterator_client;
class Request;
class multiplexing
{
private:
	struct sockaddr_in adress;
	int wait_fd, set_socket;
	int *server_socket;
	int fd_client;
	int epoll_fd;
	struct epoll_event event, event_wait[1024];
	int flg_remv;

public:
	map<int, Request> request;
	map<int, pair<string, string> > server_book;
	multiplexing(servers &config);
	~multiplexing();
	void setup_server_socket(servers &config);
	void run(servers &config);
	// Get Methode======================
	void fill_content_type();
	std::map<std::string, std::string> cont_type;
	// std::map<std::string, std::string> status_code;
};
