#include "servers.hpp"

servers::servers(const string &av)
{
	ifstream config_fd(av.c_str());

	while (!config_fd.eof())
	{
		server.push_back(server_config(config_fd));
	}
	for (server_iterator it = server.begin(); it != server.end(); it++)
	{
		int port = it->get_port();
		string server_name = it->get_server_name();
		string host =  it->get_host();
		if (it->get_host().empty() || it->get_port() == -1 || it->get_server_name().empty())
			throw (runtime_error("Invalid configue file!"));
		for(server_iterator it2 = it + 1; it2 != server.end(); it2++)
		{
			// cout << it2->get_port() <<endl;
			if ((it2->get_host() == host && it2->get_port() == port) && it2->get_server_name() == server_name)
				throw (runtime_error("Invalid configue file!"));
			if (it2->get_host().empty() || it2->get_port() == -1 || it2->get_server_name().empty())
				throw (runtime_error("Invalid configue file!"));
		}
	}
	
}

int get_right_index(vector<server_config> server, int port,string host, string server_name)
{
	int i = 0;

	for (server_iterator it = server.begin(); it != server.end(); it++,i++)
	{
		if (it->get_port() == port && it->get_host() == host)
			return (i);
		if (server_name == it->get_server_name() && it->get_port() == port )
			return (i);
	}
	return (0);
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

bool servers::get_loc_path_location(const string &name)
{
	server_iterator it = server.begin();
	for (; it != server.end(); it++)
	{
		if (it->get_loc_path_location(name) == name)
			return(true);
	}
	return false;
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
	if (i > (int)server.size())
		return server[0];
	return server[i];
}
servers::~servers()
{}