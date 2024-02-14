#include "multiplexing.hpp"
#include "request.hpp"

void Request::cgi_handle_get(epoll_event &event, servers &config)
{
	int status;
	(void)config;

	if (Path.find("cgi") != std::string::npos)
	{
		int out;
		struct timeval end;
		if ((out = waitpid(pid, &status, WNOHANG)) == 0)
		{
			get_to_cgi = true;
			gettimeofday(&end, NULL);
			double timeOut = static_cast<double>(((end.tv_sec) - (start_cgi.tv_sec)));
			if (timeOut >= 5)
			{
				// cout << fixed << timeOut << endl;
				if (pid != 0)
					kill(pid, SIGKILL);
				// error_page(event, "504", config);
				status_pro = "504";
			}
		}
		else if (WEXITSTATUS(status) != 0 && pid != 0)
		{
			cout << "CGI ERROR" << endl;
			cout << pid << endl;
			cout << WEXITSTATUS(status) << endl;
			// error_page(event, "500", config);
			status_pro = "500";
		}
		else
		{
			get_to_cgi = false;
			if (op_cgi.is_open())
			{
				read_for_send(event, cont_type, 1);
				if (op_cgi.eof())
				{
					end_of_file(event);
					return ;
				}
			}//check if the file not open
		}
	}
	check_req = 1;
}

void Request::find_cgi(servers &config, int index)
{
	if (cgi_file.empty() == true && get_to_cgi == false)
	{
		execute_cgi(*this, config[index]);
		// cout << "----------------------\n";
		// cout << cgi_file << endl;
		// cout << "----------------------\n";
		op_cgi.open(cgi_file.c_str());
	}
}