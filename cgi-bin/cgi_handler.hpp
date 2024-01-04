#pragma once

#include "../pars_config/servers.hpp"
#include "../multiplexing/multiplexing.hpp"
class Request;
class cgi_handler
{
	public:
	static int i;
		map<string, string> CGI_BOOK;
		cgi_handler(Request &req);
		~cgi_handler();
};

void php_cgi(Request &req, server_config &config);


	// env.push_back("HTTP_COOKIE=");

	//waitpid NO_HANG send response;
	//cooki on header request;