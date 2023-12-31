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
#include <stdlib.h>
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
        string methode;
        string fir_body;
        string read_request;
        string _host, _port;
        int check_first_line;
        int kk;
        int size_read;
        int size_read_request;
        long size_request;
        int check_left_header;
        int check_create_file;
        ofstream outputFile;
        void create_file(ofstream& outputFile,   map<string, string>& map);
        int parse_line(string line, int check_first);
        int parce_request(string header);
        map<string, string>  header_request;
        // map<int, pair<string, string> > server_book;
        Request(map<int, pair<string, string> > server_book, int fd_client);
        Request();
        Request(const Request& obj);
        //==========GET==========================
        string Path;
        string Protocole;
        string file_get;
        string full_Path;
        //Config_file :
        string Host, server_name;
        string Port;
        string Path_bef;
        
        void parse_url_prot();
        void redirection_content_backSlash(epoll_event &event, int epoll_fd, servers &config);
        void Generate_req_first(epoll_event &event, servers &config, int epoll_fd, map<string, string> &m);
        void Generate_req_second(epoll_event &event, int epoll_fd);
        void error_page(epoll_event &event, int epoll_fd, string key);
        // string get_content_type(string s, map<string, string>& m);
        string get_content_type(map<string, string>& m);
        void redirection_content(epoll_event &event, int epoll_fd, servers &config, int index);
        void root_page(epoll_event &event, int epoll_fd, string Pat);
        string read_buff(map<string, string> &m);
        map<string, string> status_code;
        void fill_status_code();
        string get_status_code(string key);
        void read_for_send(map<string, string> &m);
        void end_of_file(epoll_event &event, int epoll_fd);
        void close_dir();

        string fin_or_still;
        size_t check_first_time;
        int check_req;
        ifstream op;
        string exten;
        string con_type;
        string line;
        DIR* dire;
        size_t len;
        streampos fileSize;
        int ck;
        int event_fd;
};
