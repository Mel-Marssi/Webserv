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
#include <stdio.h>
#include <sys/time.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
class Request
{
public:
    bool timeOut;
    bool im_reading0;
    int add_header_response;
    bool cgi_post;
    int fd_cgi;
    int flag_read_cgi;
    int flg_pars_url;
    int kk;
    int zompie;
    struct timeval start_cgi;
    struct timeval startTime;
    struct timeval end;
    string _host, _port;
    std::string methode;
    std::string fir_body;
    std::string URL;
    std::string read_request;
    string first_line_reque;
    int size_read;
    size_t size_read_request;
    int size;
    int total;
    int rest;
    size_t size_request;
    int check_left_header;
    int check_create_file;
    int size_chunked;
    int finir;
    int acces_read_in_post;
    int next_chunked;
    int err;
    int wait;
    int last;
    std::vector<string> files;
    size_t size_File_boundri;
    string type;
    string file_name_post;
    string fake_bondary;
    string boudri;
    string last_boundri;
    string path_post;
    long size_chuked;
    void boundaries(servers &config, int index, int fd, epoll_event &event);
    void boundar(servers &config, int index);
    int check_space_first_line();
    string generat_name(string name, servers &config, int index, string content);

    // string filename;
    void create_file_bondar(ofstream &outputFile, map<string, string> &map, servers &config, int index, string name, string content);

    std::ofstream outputFile;
    void create_file(ofstream &outputFile, map<string, string> &map, servers &config, int index);
    int parse_line(string line, int check_first);
    int parce_request(string read_request, epoll_event &event, int epoll_fd, servers &config);
    map<string, string> header_request;
    void post(int fd, servers &config, epoll_event &event);
    void binary(servers &config, int index);
    void chunked(servers &config, int index);
    void default_error(string key, int fd);
    // void get_path();
    void fill_content_type();
    int Handle_error(int fd, servers &config, epoll_event &event);
    std::map<std::string, std::string> cont_type;

    Request(map<int, pair<string, string> > server_book, int fd_client);
    Request();
    Request(const Request &obj);
    string resp_post();
    //==========GET==========================

    string Path;
    size_t size_body_get;
    int check_read_get;
    string status_pro;
    string root;
    string file_get;
    string full_Path;
    string Query_String;
    string cgi_file;
    size_t size_cgi;
    int read_get;
    clock_t start;
    int fd[2];
    ssize_t eof_cgi;
    // Config_file :
    string Host, server_name;
    string Port;
    string Path_bef;

    int parse_url_prot(string meth, servers &config);
    void Generate_req_first(epoll_event &event, servers &config, map<string, string> &m);
    void Generate_req_second(epoll_event &event);
    void error_page(epoll_event &event, string key, servers &config);
    // string get_content_type(string s, map<string, string>& m);
    string get_content_type(map<string, string> &m);
    void root_page(epoll_event &event, string Pat);
    string read_buff(map<string, string> &m, string status);
    map<string, string> status_code;
    void fill_status_code();
    string get_status_code(string key);
    void read_for_send(epoll_event &event, map<string, string> &m, int flg);
    void end_of_file(epoll_event &event);
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
    DIR *dire;
    size_t len;
    streampos fileSize;
    int epoll_fd_tmp;
    int ck;
    int event_fd;
    int fd_request;
    int index_serv;
    int flg_err_page;
    int end_file_op;
    //============= Delete =========================
    int flg_entr_frst;
    int flg_resp_err;
    void Delete_Function(epoll_event &event, servers &config);
    int check_permission_F(string str);
    int check_permission_X(string str);
    void delete_content(string pat);
    int is_open_diir(string str);
    int is_open_fil(string str);
    void response_for_delete(epoll_event &event);
    string get_the_p(servers &config, string Path);
    //=============EPOLL IN OUT =====================
    string buffer;
    void cgi_handle_get(epoll_event &event, servers &config);
    void find_cgi(servers &config, int index);
    void check_url_encoding(string &str);
    char enco_form_txt(string str);
    int check_body_get(epoll_event &event);
    int flg_send;
    void check_files_open(epoll_event &event, map<string, string> m, string str);
    void Get_methode(servers &config, epoll_event &event, map<string, string> &m);
    void redirection_content(epoll_event &event, servers &config, string str, int flg);
    string get_root(servers &config);
    int delete_checker(servers &config);
    void handle_Path(size_t i);
    string handle_Path_location(string str, string str2);
    string real_Path(string str);
    string check_location_subfolder(servers &config, string str_path);
    int check_permission_R(string str);
};
