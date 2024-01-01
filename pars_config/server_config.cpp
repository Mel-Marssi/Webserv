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
	for(int i = 0; i < (int)strlen(av);i++)
	{
		if(av[i] < '0' || av[i] > '9')
			throw(runtime_error(message)); 
	}
	tmp =  atol(av);

	if(tmp != 503 && tmp != 500 && tmp != 100 && tmp != 101
		&& tmp != 501 && tmp != 404 &&tmp != 403 && tmp != 401
		&& tmp != 400 &&tmp != 304 && tmp != 302 && tmp !=  301 && tmp != 300
		&& tmp !=  200 && tmp != 201 && tmp !=  204)
		throw(runtime_error(message));
	return (static_cast<int>(tmp));
}

server_config::server_config(ifstream &config_fd)
{
	string file, word;
	// int fd_tmp;
	int flag = 0;
	this->port = -1;
	host = "";
	root = "";
	server_name= "";
	index = "";
	server_auto_index = false;
	// error_book();
	while (getline(config_fd, file))
	{
		istringstream cscan(file);
		cscan >> word;
		if(word == "}")
			break;
		if (word == "server" && flag == 0)
		{
			flag = 1;
			continue;
		}
		else if (flag == 0 || (flag ==1 && word == "server"))
			throw(runtime_error("Invalid configue file!"));
		if (word.empty() || file.empty())
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
			cscan >> word;
			if (get_server_name().empty() == false)
				throw(runtime_error("Invalid configue file!"));
			server_name = word;
			cscan >> word;
			if (word != ";")
				throw runtime_error("Invalid configue file!");
		}
		else if (word == "host")
		{
			cscan >>word;
			if (get_host().empty() == false)
				throw(runtime_error("Invalid configue file!"));
			host = word;
			cscan >> word;
			if (word != ";")
				throw runtime_error("Invalid configue file!");
		}
		else if(word == "root")
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
			if (get_loc_path_location(word) == word)
				throw(runtime_error("Invalid configue file!"));
			this->serv_locations.push_back(location(config_fd, word));
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
	// error_book_iterator it =  _error_book.begin();
	// for(; it != _error_book.end(); it++)
	// 	cout << it->first <<" "<<it->second<<endl;
	// cout << "-----------------------------------------------------------" <<endl;
	// cout << "Port : " <<  port << "\n";
	// cout << "server name: " << server_name << "\n" <<host << "\n";
	// cout << "root: " << root << "\n";
	// cout << "auto index : " << server_auto_index << "\n"; 
	// cout << "index: " << index <<endl;
	// for(int i = 0; i < (int)serv_locations.size();i++)
	// {
	// 	cout << serv_locations[i] <<endl;
	// }
	
	// cout << "-----------------------------------------------------------" <<endl;
}


bool server_config::get_loc_delete(const string &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get_delete());
	}
	return (false);
}

bool server_config::get_loc_post(const string &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get_post());
	}
	return (false);
}

bool server_config::get_loc_get(const string &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get());
	}
	return (false);
}

bool server_config::get_loc_auto_index(const string &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get_auto_index());
	}
	return (false);
}

bool server_config::get_loc_upload(const string &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get_upload());
	}
	return (false);
}
string server_config::get_loc_root(string const &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get_root());
	}
	return ("");
}

size_t server_config::get_loc_max_client_size(const string &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name){
			return(it->get_max_client_size());
		}
	}
	return (0);
}

string server_config::get_loc_up_folder(string const &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get_upload_folder());
	}
	return ("");
}

string server_config::get_loc_index(const string  &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get_index());
	}
	return ("");
}

string server_config::get_loc_redirection(const string  &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get_redirection());
	}
	return ("");
}

string server_config::get_loc_cgi_path(const string  &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get_cgi_path());
	}
	return ("");
}

string server_config::get_loc_path_location(const string  &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get_path_location());
	}
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

string server_config::get_server_name()
{
	return (server_name);
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
