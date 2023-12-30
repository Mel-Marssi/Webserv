#pragma once

#include "request.hpp"
#include <arpa/inet.h>

typedef vector<int>::iterator iterator_client;
class multiplexing
{
	private:

	public:
		map<int, Request>  request;
		map<int, pair<string, string> > server_book;
		multiplexing( servers &config);
		~multiplexing();

		//Get Methode======================
        void fill_content_type();
        std::map<std::string, std::string> cont_type;
        // std::map<std::string, std::string> status_code;
};


