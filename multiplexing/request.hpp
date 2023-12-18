#pragma once

#include "../pars_config/servers.hpp"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <cstdlib>  
#include <string>
#include <iostream>
#include <sstream>
#define POST "POST"
#define GET "GET"
#define DELETE "DELETE"
#define NONE "NONE"

class Request
{
    public:
        std::string methode;
        std::string fir_body;
        std::string URL;
        std::string Protocole;
        std::string read_request;
        int size_read;
        int size_read_request;
        long size_request;
        int check_left_header;
        int check_create_file;
        std::ofstream outputFile;
        void create_file(std::ofstream& outputFile,   std::map<std::string, std::string>& map);
        int parse_line(std::string line, int check_first);
		int parce_request(std::string header);
		std::map<std::string, std::string>  header_request;
        Request();
        Request(const Request& obj);
};
