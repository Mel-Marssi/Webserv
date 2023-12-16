#include "server_config.hpp"

server_config::server_config(ifstream &config_fd)
{
	string file, word;
	int flag = 0;
	port = -1;
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
		}
		else if (word == "server_name")
		{
			cscan >> word;
			if (!get_server_name().empty())
				throw(runtime_error("Invalid configue file!"));
			server_name = word;
		}
		else if (word == "host")
		{
			cscan >>word;
			if (!get_host().empty())
				throw(runtime_error("Invalid configue file!"));
			host = word;
		}
		else if(word == "root")
		{
			cscan >> word;
			if (!get_root().empty())
				throw(runtime_error("Invalid configue file!"));
			root = word;
		}
		else if (word == "index")
		{
			cscan >> word;
			if (!get_index().empty())
				throw(runtime_error("Invalid configue file!"));
			index = word;
			word.erase();
			file.erase();
		}
		else if (word == "location")
		{
			cscan >> word;
			if (get_loc_path_location(word) != "false")
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
			word.erase();
			file.erase();
		}
		else if (!word.empty())
			throw runtime_error("Invalid configue file!");
	}
	cout << "-----------------------------------------------------------" <<endl;
	cout << "Port : " <<  port << "\n";
	cout << "server name: " << server_name << "\n" <<host << "\n";
	cout << "root: " << root << "\n";
	cout << "auto index : " << server_auto_index << "\n"; 
	cout << "index: " << index <<endl;
	for(int i = 0; i < (int)serv_locations.size();i++)
	{
		cout << serv_locations[i] <<endl;
	}
	
	cout << "-----------------------------------------------------------" <<endl;
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

string server_config::get_loc_root(string const &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get_root());
	}
	return ("false");
}

string server_config::get_loc_index(const string  &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get_index());
	}
	return ("false");
}

string server_config::get_loc_redirection(const string  &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get_redirection());
	}
	return ("false");
}

string server_config::get_loc_cgi_path(const string  &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get_cgi_path());
	}
	return ("false");
}

string server_config::get_loc_path_location(const string  &name)
{
	location_iterator it = serv_locations.begin();
	for(; it  != serv_locations.end(); it++)
	{
		if (it->get_path_location() == name)
			return(it->get_path_location());
	}
	return ("false");
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
