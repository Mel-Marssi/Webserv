#include "multiplexing.hpp"
#include "request.hpp"

void Request::cgi_handle_get(epoll_event &event, servers &config)
{
	int status;
	if (Path.find("cgi") != std::string::npos)
	{
		int out;
		//  start = clock();
		if ((out = waitpid(pid, &status, WNOHANG)) == 0)
		{
			get_to_cgi = true;
			double tmp = (clock() - start) / CLOCKS_PER_SEC;
			if (tmp >= 30)
			{
				kill(pid, SIGKILL);
				error_page(event, "504", config);
			}
		}
		// else if ( WEXITSTATUS(status) != 0 && pid != 0)
		// {
		// 	// cout << "CGI ERROR" << endl;
		// 	// cout << pid << endl;
		// 	// cout << WEXITSTATUS(status) << endl;
		// 	error_page(event, "500", config);
		// }
		else
		{
			get_to_cgi = false;
			if (op_cgi.is_open())
			{

				read_for_send(event, cont_type, 1);
				if (op_cgi.eof())
				{
					end_of_file(event);
				}
			}
		}
	}
}

void Request::find_cgi(servers &config, int index)
{
	if (cgi_file.empty() == true && get_to_cgi == false)
	{
		php_cgi(*this, config[index]);
		op_cgi.open(cgi_file.c_str());
	}
}