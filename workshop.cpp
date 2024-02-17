#include "pars_config/servers.hpp"
#include "multiplexing/multiplexing.hpp"

int main(int ac, char **av)
{
	string config_file;
	if (ac == 2)
	{
		config_file = (av[1]);
	}
	else
	{
		config_file = ("pars_config/config.file");
	}
	try
	{
		servers configs(config_file);
		configs.run_checks();
		
		multiplexing server(configs);
		server.setup_server_socket(configs);
		server.run(configs);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		// sleep(5);
	}
}