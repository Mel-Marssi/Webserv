#pragma once

#include "request.hpp"

typedef vector<int>::iterator iterator_client;
class multiplexing
{
	private:

	public:
		std::map<int, Request>  request;
		multiplexing( servers &config);
		~multiplexing();

		//Get Methode======================
        void fill_content_type();
        std::map<std::string, std::string> cont_type;
        // std::map<std::string, std::string> status_code;
};


