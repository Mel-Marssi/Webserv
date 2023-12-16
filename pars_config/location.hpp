#pragma once
#include "server_config.hpp"
#include <sstream>


using namespace std;
class location
{
	private:
		bool _delete;
		bool _post;
		bool _get;
		bool _auto_index;
		string root;
		string index;
		string redirection;
		string cgi_path;
		string path_location;
	public:
		location(ifstream &config_fd, string &tmp);
		bool get_delete() const;
		bool get_post() const;
		bool get() const;
		bool get_auto_index() const;
		string get_root() const;
		string get_index() const;
		string get_redirection() const;
		string get_cgi_path() const;
		string get_path_location() const;
		~location();
};

int check_atoi(const char *av, const char *message);
ostream &operator<<(ostream &var,const location &obj);
