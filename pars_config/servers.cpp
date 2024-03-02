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
	if (!config_fd.is_open())
		throw(runtime_error("Invalid configue file!"));
	while (!config_fd.eof())
	{
		server.push_back(server_config(config_fd));
	}
}
void servers::run_checks()
{
	for (server_iterator it = server.begin(); it != server.end(); it++)
	{
		if (it->get_host().empty() || it->get_port() == -1)
			throw(runtime_error("Missing Server Name or Host or Port!"));
		int port = it->get_port();
		string host = it->get_host();

		for (server_iterator it2 = it + 1; it2 != server.end(); it2++)
		{
			if (it2->get_host() == host && it2->get_port() == port && hasDuplicate(it->get_server_names(), it2->get_server_names()) == true)
				throw(runtime_error("Duplicate server!"));
			else if (it2->get_host() == host && it2->get_port() == port && it2->get_server_names().size() == 0 && it->get_server_names().size() == 0)
				throw(runtime_error("Duplicate server!"));
			if (it2->get_host().empty() || it2->get_port() == -1)
				throw(runtime_error("Invalid configue file!"));
		}
	}
}
int get_right_index(vector<server_config> server, int port, string host, string server_name)
{
	int i = 0;
	vector<string> tmp_server_name;
	if (server_name.find("\r") != string::npos)
		server_name.erase(server_name.find("\r"), 1);
	if (host.find("\r") != string::npos)
		host.erase(host.find("\r"), 1);
	if (server_name == "localhost")
		server_name = "127.0.0.1";
	for (server_iterator it = server.begin(); it != server.end(); it++, i++)
	{
		tmp_server_name = it->get_server_names();
		if (it->get_port() == port && it->get_host() == host)
		{
			if (server_name.empty() || server_name == host)
				return i;
			if (find(tmp_server_name.begin(), tmp_server_name.end(), server_name) != tmp_server_name.end())
				return (i);
		}
	}
	return (-1);
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