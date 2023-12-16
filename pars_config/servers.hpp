#include "server_config.hpp"

typedef vector<server_config>::iterator server_iterator;
class servers
{
	private:
		vector<server_config> server;
	public:
		servers(const string &av);
		int get_port(int server_key);
		string get_root(int server_key);
		string get_server_name(int server_key);
		string get_index(int server_key);
		bool get_loc_delete(const string &name);
		bool get_loc_post(const string &name);
		bool get_loc_get(const string &name);
		bool get_loc_auto_index(const string &name);
		string get_loc_root(const string &name);
		string get_loc_index(const string &name);
		string get_loc_redirection(const string &name);
		string get_loc_cgi_path(const string &name);
		~servers();
};
