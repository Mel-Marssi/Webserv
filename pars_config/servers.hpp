#pragma once
#include "server_config.hpp"

typedef vector<server_config>::iterator server_iterator;
typedef vector<string>::iterator string_iterator;
class servers
{
	
	public:
		vector<server_config> server;
		servers(const string &av);
		void run_checks();
		server_config &operator[](int i);
		int size();
		~servers();
};

int get_right_index(vector<server_config> server, int port, string host, string server_name);

