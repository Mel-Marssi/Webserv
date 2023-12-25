#include"pars_config/servers.hpp"
#include "multiplexing/multiplexing.hpp"
int main(int ac , char **av)
{
	string tmp;
	if (ac == 2)
		tmp = av[1];
	else
		tmp = "pars_config/config.file";
	try
	{
		servers configs(tmp);
		multiplexing tmp(configs);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}

//printf "GET /test/fich.pdf  HTTP/1.1 \r\n\r\n" | nc localhost 1337
