#pragma once

#include "../pars_config/servers.hpp"
#include "../cgi-bin/cgi_handler.hpp"
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
#include <fcntl.h>
#include <sys/wait.h>
#define POST "POST"
#define GET "GET"
#define DELETE "DELETE"
#define NONE "NONE"
#define Still "Still"
#define finish "finish"


#include <cstdio>
#include <sys/stat.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
class Request
{
    public:
        int kk;
        // string methode;
        // string fir_body;
        // string read_request;
        string _host, _port;
        bool get_to_cgi;
        // int check_first_line;
        // int size_read;
        // int size_read_request;
        // long size_request;
        // int check_left_header;
        // int check_create_file;
        // ofstream outputFile;
        std::string methode;
        std::string fir_body;
        std::string URL;
        // std::string Protocole;
        std::string read_request;
        int size_read;
        int size_read_request;
        int size ;
        int total;
        long size_request;
        int check_left_header;
        int check_create_file;
        int size_chunked;
        int finir;
        int acces_read_in_post;
        int next_chunked;
        int err;
        int wait;
        int last;
        string fake_bondary;
        string boudri;
        string last_boundri;
        void boundaries(servers &config, int index,int fd);
        void boundar(servers &config, int index);

        // string filename;
        void create_file_bondar(ofstream& outputFile,   map<string, string>& map, servers &config, int index,string name,  string content);

        std::ofstream outputFile;
        void create_file(ofstream& outputFile,   map<string, string>& map, servers &config, int index);
        int parse_line(string line, int check_first);
        int parce_request(string read_request, epoll_event &event, int epoll_fd, servers &config);
        map<string, string>  header_request;
        void  post(int fd, servers &config, epoll_event &event);
        void binary(servers &config, int index);
        void chunked(servers &config, int index);
        void default_error(string key, int fd);
        // void get_path();
        void fill_content_type();
        std::map<std::string, std::string> cont_type;

        Request(map<int, pair<string, string> > server_book, int fd_client);
        Request();
        Request(const Request& obj);
        //==========GET==========================
        string Path;
        string Protocole;
        string file_get;
        string full_Path;
        string Query_String;
        string cgi_file;
        clock_t start;
        int fd[2];
        ssize_t eof_cgi;
        //Config_file :
        string Host, server_name;
        string Port;
        string Path_bef;
        
        void parse_url_prot(string meth, int epoll_fd, epoll_event &event, servers &config);
        void redirection_content_backSlash(epoll_event &event, int epoll_fd, int flg);
        void Generate_req_first(epoll_event &event, servers &config, int epoll_fd, map<string, string> &m);
        void Generate_req_second(epoll_event &event, int epoll_fd);
        void error_page(epoll_event &event, int epoll_fd, string key, servers &config);
        // string get_content_type(string s, map<string, string>& m);
        string get_content_type(map<string, string>& m);
        void redirection_content(epoll_event &event, int epoll_fd, servers &config, int index);
        void root_page(epoll_event &event, int epoll_fd, string Pat);
        string read_buff(map<string, string> &m);
        map<string, string> status_code;
        void fill_status_code();
        string get_status_code(string key);
        void read_for_send(map<string, string> &m, int flg);
        void end_of_file(epoll_event &event, int epoll_fd);
        void close_dir();
        std::string read_buff_cgi(map<string, string> &m);

        string fin_or_still;
        size_t check_first_time;
        int check_req;
        ifstream op;
        ifstream op_cgi;
        pid_t pid;
        string exten;
        string con_type;
        string line;
        DIR* dire;
        size_t len;
        streampos fileSize;
        int epoll_fd_tmp;
        int ck;
        int event_fd;
        //============= Delete =========================
        void Delete_Function(epoll_event &event, servers &config, int epoll_fd, map<string, string> &m);
        int check_permission_F(string str);
        int check_permission_X(string str);
        void delete_content(string pat, string file, epoll_event& event, int epoll_fd);
        int is_open_diir(string str);
        int is_open_fil(string str);
        void response_for_delete(string status, epoll_event &event, int epoll_fd);
        string get_the_p(servers &config, string Path, string file_get);
};
