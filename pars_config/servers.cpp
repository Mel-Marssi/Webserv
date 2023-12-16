#include "servers.hpp"

servers::servers(const string &av)
{
	ifstream config_fd(av.c_str());

	while (!config_fd.eof())
	{
		server.push_back(server_config(config_fd));
	}
}


bool servers::get_loc_auto_index(const string &name)
{
	server_iterator it = server.begin();
	for (; it != server.end(); it++)
	{
		if (it->get_loc_path_location(name) == name)
			return(it->get_loc_auto_index(name));
	}
	return false;
}

bool servers::get_loc_delete(const string &name)
{
	server_iterator it = server.begin();
	for (; it != server.end(); it++)
	{
		if (it->get_loc_path_location(name) == name)
			return(it->get_loc_delete(name));
	}
	return false;
}

bool servers::get_loc_post(const string &name)
{
	server_iterator it = server.begin();
	for (; it != server.end(); it++)
	{
		if (it->get_loc_path_location(name) == name)
			return(it->get_loc_post(name));
	}
	return false;
}

bool servers::get_loc_get(const string &name)
{
	server_iterator it = server.begin();
	for (; it != server.end(); it++)
	{
		if (it->get_loc_path_location(name) == name)
			return(it->get_loc_get(name));
	}
	return false;
}

string servers::get_loc_index(const string &name)
{
	server_iterator it = server.begin();
	for (; it != server.end(); it++)
	{
		if (it->get_loc_path_location(name) == name)
			return(it->get_loc_index(name));
	}
	return "false";
}
string servers::get_loc_root(const string &name)
{
	server_iterator it = server.begin();
	for (; it != server.end(); it++)
	{
		if (it->get_loc_path_location(name) == name)
			return(it->get_loc_root(name));
	}
	return "false";
}

string servers::get_loc_redirection(const string &name)
{
	server_iterator it = server.begin();
	for (; it != server.end(); it++)
	{
		if (it->get_loc_path_location(name) == name)
			return(it->get_loc_redirection(name));
	}
	return "false";
}

string servers::get_loc_cgi_path(const string &name)
{
	server_iterator it = server.begin();
	for (; it != server.end(); it++)
	{
		if (it->get_loc_path_location(name) == name)
			return(it->get_loc_cgi_path(name));
	}
	return "false";
}

int servers::get_port(int server_key)
{
	server_iterator it = server.begin();
	for (; it != server.end(); it++)
	{
		if (it->get_port() == server_key)
			return(it->get_port());
	}
	return (-1);
}

string servers::get_root(int server_key)
{
	server_iterator it = server.begin();
	for (; it != server.end(); it++)
	{
		if (it->get_port() == server_key)
			return(it->get_root());
	}
	return ("false");
}

string servers::get_server_name(int server_key)
{
	server_iterator it = server.begin();
	for (; it != server.end(); it++)
	{
		if (it->get_port() == server_key)
			return(it->get_server_name());
	}
	return ("false");
}


string servers::get_index(int server_key)
{
	server_iterator it = server.begin();
	for (; it != server.end(); it++)
	{
		if (it->get_port() == server_key)
			return(it->get_index());
	}
	return ("false");
}

int servers::size()
{
	return ((int)server.size());
}
server_config &servers::operator[](int i)
{
	return server[i];
}
servers::~servers()
{}