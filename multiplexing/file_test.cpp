#include"pars_config/servers.hpp"
#include "multiplexing/multiplexing.hpp"

int main(int ac , char **av)
{
	(void)av;(void)ac;
	string tmp;
	tmp = "pars_config/config.file";
	try
	{
		servers configs(tmp);
		multiplexing tmp(configs);
	}
	catch(const std::exception& e)
	{std::cerr << e.what() << '\n';}
	
}