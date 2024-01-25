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
		bool _allow_upload;
		size_t max_client_size;
		string root;
		string index;
		string redirection;
		string cgi_path;
		vector<string> cgi_exec_path;
		// string path_location;
		string uploads_folder;
	public:
		location();
		location(ifstream &config_fd, string word_serv);
		size_t get_max_client_size() const;
		bool get_delete() const;
		bool get_post() const;
		bool get() const;
		bool get_auto_index() const;
		bool get_upload() const;
		vector<string> get_cgi_exec_path() const;
		string get_root() const;
		string get_upload_folder() const;
		string get_index() const;
		string get_redirection() const;
		string get_cgi_path() const;
		// string get_path_location() const;
		~location();
};

int check_atoi(const char *av, const char *message);
ostream &operator<<(ostream &var,const location &obj);
void check_size_t(const char *av, const char *message);