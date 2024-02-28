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
	int read_request(int event_fd, servers &config, int i);
	void error_epoll(int event_fd, int i);
	void delete_method(int event_fd, servers &config, int i);
	int send_response(int event_fd, servers &config, int i);
	int get_methode(int event_fd, servers &config, int i);
	void time_out_post(int event_fd, servers &config, int i);
	void post_boundry(int event_fd, servers &config, int i);
	int accept_client(int event_fd);
	void cgi_post(int event_fd, servers &config, int i);
	// Get Methode======================
	void fill_content_type();
	std::map<std::string, std::string> cont_type;
	// std::map<std::string, std::string> status_code;
};

