#include "servers.hpp"

bool hasDuplicate(vector<string> vec1, vector<string> vec2)
{

	for (string_iterator it1 = vec1.begin(); it1 != vec1.end(); ++it1)
	{
		if (find(vec2.begin(), vec2.end(), *it1) != vec2.end())
		{
			return true;
		}
	}
	return false;
}
servers::servers(const string &av)
{
	ifstream config_fd(av.c_str());

	while (!config_fd.eof())
	{
		server.push_back(server_config(config_fd));
	}
	for (server_iterator it = server.begin(); it != server.end(); it++)
	{
		if (it->get_host().empty() || it->get_port() == -1)
			throw(runtime_error("Missing Server Name or Host or Port!"));
		int port = it->get_port();
		// string server_name = it->get_server_names()[];
		string host = it->get_host();

		for (server_iterator it2 = it + 1; it2 != server.end(); it2++)
		{
			// cout << it2->get_port() <<endl;
			if (it2->get_host() == host && it2->get_port() == port && hasDuplicate(it->get_server_names(), it2->get_server_names()) == true)
				throw(runtime_error("Invalid configue file!"));
			if (it2->get_host().empty() || it2->get_port() == -1)
				throw(runtime_error("Invalid configue file!"));
		}
	}
}

int get_right_index(vector<server_config> server, int port, string host, string server_name)
{
	int i = 0;
	int tmp_serv = -1;
	vector<string> tmp_server_name;

	for (server_iterator it = server.begin(); it != server.end(); it++, i++)
	{
		tmp_server_name = it->get_server_names();
		if (it->get_port() == port && it->get_host() == host)
		{
			if (tmp_serv == -1)
				tmp_serv = i;
			if (find(tmp_server_name.begin(), tmp_server_name.end(), server_name) != tmp_server_name.end())
				return (i);
		}
	}
	if(tmp_serv != -1)
		return (tmp_serv);
	return (0);
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
{
}