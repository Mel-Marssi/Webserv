#include"pars_config/servers.hpp"

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
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}