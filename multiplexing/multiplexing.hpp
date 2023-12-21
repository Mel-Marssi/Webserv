#pragma once
#include <arpa/inet.h>

#include "request.hpp"

typedef vector<int>::iterator iterator_client;
class multiplexing
{
	private:

	public:
	 	
		std::map<int, Request>  request;
		multiplexing( servers &config);
		~multiplexing();
		
};


