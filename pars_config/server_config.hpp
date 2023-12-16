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
#include "location.hpp"

using namespace std;

class location;
typedef vector<location>::iterator location_iterator;
class server_config
{
	private:
		int port;
		string host;
		string root;
		string server_name;
		string index;
		bool server_auto_index;
		vector<location> serv_locations;
	public:
		server_config(ifstream &config_fd);
		int get_port();
		bool get_server_auto_index();
		string get_root();
		string get_server_name();
		string get_index();
		string get_host();
		bool get_loc_delete(const string &name);
		bool get_loc_post(const string &name);
		bool get_loc_get(const string &name);
		bool get_loc_auto_index(const string &name);
		string get_loc_root(const string &name);
		string get_loc_index(const string &name);
		string get_loc_redirection(const string &name);
		string get_loc_cgi_path(const string &name);
		string get_loc_path_location(const string &name);
		~server_config();
};
