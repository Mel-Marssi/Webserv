#pragma once

#include "../pars_config/servers.hpp"
#include <sys/socket.h>
#include <sys/epoll.h>

typedef vector<int>::iterator iterator_client;
class multiplexing
{
	private:

	public:
		multiplexing( servers &config);
		~multiplexing();
};


