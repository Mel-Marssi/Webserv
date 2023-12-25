#pragma once

#include "../pars_config/servers.hpp"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <cstdlib>  
#include <string>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <deque>
#include <sstream>
#include <sys/stat.h>
#define POST "POST"
#define GET "GET"
#define DELETE "DELETE"
#define NONE "NONE"
#define Still "Still"
#define finish "finish"

class Request
{
    public:
        std::string methode;
        std::string fir_body;
        std::string read_request;
        int check_first_line;
        int kk;
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
        //==========GET==========================
        std::string Path;
        std::string Protocole;
        std::string file_get;
        std::string full_Path;
        
        void parse_url_prot();
        void Generate_req_first(epoll_event &event, servers &config, int epoll_fd, map<string, string> &m);
        void Generate_req_second(epoll_event &event, int epoll_fd);
        void error_page(epoll_event &event, int epoll_fd);
        // std::string get_content_type(string s, map<string, string>& m);
        std::string get_content_type(map<string, string>& m);
        void redirection_content(epoll_event &event, int epoll_fd, servers &config);
        void root_page(epoll_event &event, int epoll_fd, std::string Pat);


        std::string fin_or_still;
        size_t check_first_time;
        int check_req;
        std::ifstream op;
        std::string exten;
        std::string con_type;
        string line;
        DIR* dire;
        int entre_or_not;
        size_t len;
        streampos fileSize;
        int ck;
};
