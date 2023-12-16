#pragma once

#include "../pars_config/servers.hpp"
#include <sys/socket.h>
#include <sys/epoll.h>

class multiplexing
{
	private:
		vector<int> fd_socket;
	public:
		multiplexing( servers &config);
		~multiplexing();
};

