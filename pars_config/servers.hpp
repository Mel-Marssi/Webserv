#pragma once
#include "server_config.hpp"

typedef vector<server_config>::iterator server_iterator;
class servers
{
	
	public:
		vector<server_config> server;
		servers(const string &av);
		// int get_port(int server_key);
		// string get_root(int server_key);
		string get_server_name(int server_key);
		// string get_index(int server_key);
		// // port and hostname and servername
		// bool get_loc_delete(const string &name);
		// bool get_loc_post(const string &name);
		// bool get_loc_get(const string &name);
		// bool get_loc_auto_index(const string &name);
		// string get_loc_root(const string &name);
		// string get_loc_index(const string &name);
		// string get_loc_redirection(const string &name);
		// string get_loc_cgi_path(const string &name);
		// bool get_loc_path_location(const string &name);
		server_config &operator[](int i);
		int size();
		~servers();
};

int get_right_index(vector<server_config> server, int port, string host, string server_name);

