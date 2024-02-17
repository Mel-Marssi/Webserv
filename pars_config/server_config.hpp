#pragma once

#include <iostream>
#include <climits>
#include <fstream>
#include <map>
#include <vector>
#include <iomanip>
#include<cmath>
#include <cstring>
#include <string>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include<algorithm>
#include "location.hpp"

using namespace std;

class location;
typedef vector<string>::iterator string_iterator;
typedef vector<location>::iterator location_iterator;
typedef map<int, string>::iterator error_book_iterator;
typedef map<string, location>::iterator location_map_iterator;
class server_config
{
	private:
		int port;
		string host;
		string root;
		// string server_name;
		vector<string> server_names;
		string index;
		bool server_auto_index;
		// vector<location> serv_locations;
		map<string, location> serv_locations;
	public:
		map<int, string> _error_book;
		server_config(ifstream &config_fd);
		void setup_server_info(ifstream &config_fd);
		// void error_book();
		int get_port();
		bool get_server_auto_index();
		string get_root();
		vector<string> get_server_names();
		string get_index();
		string get_host();
		bool get_loc_delete(const string &name);
		size_t get_loc_max_client_size(const string &name);
		bool get_loc_post(const string &name);
		bool get_loc_get(const string &name);
		bool get_loc_auto_index(const string &name);
		bool get_loc_upload(const string &name);
		string get_loc_up_folder(string const &name);
		string get_loc_root(const string &name);
		string get_loc_index(const string &name);
		string get_loc_redirection(const string &name);
		//string get_loc_cgi_path(const string &name);
		map<string, string> get_loc_cgi_exec_path(const string &name);
		string get_loc_path_location(const string &name);
		~server_config();
};
