#include "server_config.hpp"

// void server_config::error_book()
// {
// 	_error_book[100] = "error100.html";
// 	_error_book[101] = "error101.html";
// 	_error_book[200] = "error200.html";
// 	_error_book[201] = "error201.html";
// 	_error_book[204] = "error204.html";
// 	_error_book[300] = "error300.html";
// 	_error_book[301] = "error301.html";
// 	_error_book[302] = "error302.html";
// 	_error_book[304] = "error304.html";
// 	_error_book[400] = "error400.html";
// 	_error_book[401] = "error401.html";
// 	_error_book[403] = "error403.html";
// 	_error_book[404] = "error404.html";
// 	_error_book[500] = "error500.html";
// 	_error_book[503] = "error503.html";
// }

int check_error_code(const char *av, const char *message)
{
	long long tmp;
	for (int i = 0; i < (int)strlen(av); i++)
	{
		if (av[i] < '0' || av[i] > '9')
			throw(runtime_error(message));
	}
	tmp = atol(av);

	if (tmp < 100 || tmp > 599)
		throw(runtime_error(message));
	return (static_cast<int>(tmp));
}

server_config::server_config(ifstream &config_fd)
{
	this->port = -1;
	host = "";
	root = "";
	// server_name = "";
	index = "";
	server_auto_index = false;
	this->setup_server_info(config_fd);
}

void server_config::setup_server_info(ifstream &config_fd)
{
	string file, word, tmp_word;
	int flag = 0;
	while (getline(config_fd, file))
	{
		if (file.empty())
			continue;
		if (file.find(";") != string::npos)
		{
			file.insert(file.find(";"), " ");
			if (file.find(";") != file.size() - 1)
				throw(runtime_error("Character after \";\""));
		}
		istringstream cscan(file);
		cscan >> word;
		if (word == "}")
			break;
		if (word == "server" && flag == 0)
		{
			flag = 1;
			continue;
		}
		else if (flag == 0 || (flag == 1 && word == "server"))
			throw(runtime_error("Invalid configue file!"));
		if (word.empty())
			continue;
		if (word == "listen")
		{
			cscan >> word;
			if (get_port() != -1)
				throw(runtime_error("Invalid configue file!"));
			port = check_atoi(word.c_str(), "Invalid Port!");
			cscan >> word;
			if (word != ";")
				throw runtime_error("Invalid configue file!");
		}
		else if (word == "server_name")
		{
			if (file.find(";") == string::npos)
				throw(runtime_error("Invalid configue file!"));
			cscan >> word;
			while (word != ";")
			{
				server_names.push_back(word);
				cscan >> word;
			}
		}
		else if (word == "host")
		{
			cscan >> word;
			if (get_host().empty() == false)
				throw(runtime_error("Invalid configue file!"));
			if (word == "localhost")
				word = "127.0.0.1";
			host = word;
			cscan >> word;
			if (word != ";")
				throw runtime_error("Invalid configue file!");
		}
		else if (word == "root")
		{
			cscan >> word;
			if (get_root().empty() == false)
				throw(runtime_error("Invalid configue file!"));
			root = word;
			cscan >> word;
			if (word != ";")
				throw runtime_error("Invalid configue file!");
		}
		else if (word == "index")
		{
			cscan >> word;
			if (get_index().empty() == false)
				throw(runtime_error("Invalid configue file!"));
			index = word;
			cscan >> word;
			if (word != ";")
				throw runtime_error("Invalid configue file!");
			word.erase();
			file.erase();
		}
		else if (word == "location")
		{
			cscan >> word;
			tmp_word = word;
			if (serv_locations.find(tmp_word) != serv_locations.end())
				throw(runtime_error("Invalid configue file!"));
			cscan >> word;
			if (word != "{")
				throw(runtime_error("Invalid configue file!"));
			serv_locations.insert(std::make_pair(tmp_word, location(config_fd, tmp_word)));
			word.erase();
			file.erase();
		}
		else if (word == "autoindex")
		{
			cscan >> word;
			if (word == "on")
				server_auto_index = true;
			else if (word == "off")
				server_auto_index = false;
			else
				throw runtime_error("Invalid configue file!");
			cscan >> word;
			if (word != ";")
				throw runtime_error("Invalid configue file!");
			word.erase();
			file.erase();
		}
		else if (word == "error_pages")
		{
			cscan >> word;
			int key = check_error_code(word.c_str(), "Invalid Error pages!");
			cscan >> word;
			if (word == ";")
				throw runtime_error("Invalid configue file!");
			_error_book[key] = word;
			cscan >> word;
			if (word != ";")
				throw runtime_error("Invalid configue file!");
		}
		else if (!word.empty())
			throw runtime_error("Invalid configue file!");
	}
}
bool server_config::get_loc_delete(const string &name)
{
	if (serv_locations.find(name) != serv_locations.end())
		return serv_locations[name].get_delete();
	return (false);
}

bool server_config::get_loc_post(const string &name)
{
	if (serv_locations.find(name) != serv_locations.end())
		return serv_locations[name].get_post();
	return (false);
}

bool server_config::get_loc_get(const string &name)
{
	if (serv_locations.find(name) != serv_locations.end())
		return serv_locations[name].get();
	return (false);
}

bool server_config::get_loc_auto_index(const string &name)
{
	if (serv_locations.find(name) != serv_locations.end())
		return serv_locations[name].get_auto_index();
	return (false);
}

bool server_config::get_loc_upload(const string &name)
{
	if (serv_locations.find(name) != serv_locations.end())
		return serv_locations[name].get_upload();
	return (false);
}
string server_config::get_loc_root(string const &name)
{
	if (serv_locations.find(name) != serv_locations.end())
		return serv_locations[name].get_root();
	return ("");
}

size_t server_config::get_loc_max_client_size(const string &name)
{
	if (serv_locations.find(name) != serv_locations.end())
		return serv_locations[name].get_max_client_size();
	return (0);
}

string server_config::get_loc_up_folder(string const &name)
{
	if (serv_locations.find(name) != serv_locations.end())
		return serv_locations[name].get_upload_folder();
	return ("");
}

string server_config::get_loc_index(const string &name)
{
	if (serv_locations.find(name) != serv_locations.end())
		return serv_locations[name].get_index();
	return ("");
}

string server_config::get_loc_redirection(const string &name)
{
	if (serv_locations.find(name) != serv_locations.end())
		return serv_locations[name].get_redirection();
	return ("");
}

// string server_config::get_loc_cgi_path(const string &name)
// {
// 	if (serv_locations.find(name) != serv_locations.end())
// 		return serv_locations[name].get_cgi_path();
// 	return ("");
// }

map<string, string> server_config::get_loc_cgi_exec_path(const string &name)
{
	if (serv_locations.find(name) != serv_locations.end())
		return serv_locations[name].get_cgi_exec_path();
	return (map<string, string>());
}
string server_config::get_loc_path_location(const string &name)
{
	if (serv_locations.find(name) != serv_locations.end())
		return name;
	return ("");
}

int server_config::get_port()
{
	return (port);
}

string server_config::get_root()
{
	return (root);
}

// string server_config::get_server_name()
// {
// 	return (server_name);
// }
vector<string> server_config::get_server_names()
{
	return (server_names);
}
string server_config::get_index()
{
	return (index);
}

string server_config::get_host()
{
	return (host);
}

bool server_config::get_server_auto_index()
{
	return (server_auto_index);
}

server_config::~server_config()
{
}
