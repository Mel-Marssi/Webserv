#include "multiplexing.hpp"
#include "request.hpp"
// open dir need to close
//  index on cgi

void Request::cgi_handle_get(epoll_event &event, servers &config)
{
	int status;
	(void)config;
	if (Path.find("/cgi-bin") != std::string::npos)
	{
		if ((waitpid(pid, &status, WNOHANG)) == 0)
		{
			gettimeofday(&end, NULL);
			double timeOut = static_cast<double>(((end.tv_sec + end.tv_usec / 1000000) - (start_cgi.tv_sec + start_cgi.tv_usec / 1000000)));
			if (timeOut >= 30)
			{
				if (pid != 0)
				{
					kill(pid, SIGKILL);
					waitpid(pid, &status, 0);
					remove(cgi_file.c_str());
				}
				status_pro = "504";
			}
			else if (timeOut != 0)
				this->timeOut = true;
		}
		else if ((WIFEXITED(status) && WEXITSTATUS(status) != 0 && pid != 0) || WIFSIGNALED(status))
		{
			status_pro = "500";
		}
		else
		{
			if (op_cgi.is_open())
			{
				read_for_send(event, cont_type, flag_read_cgi);
				if (op_cgi.eof())
				{
					end_of_file(event);
					return;
				}
			}
		}
	}
	check_req = 1;
}

void Request::find_cgi(servers &config, int index)
{
	if (check_permission_F(full_Path) == 0)
	{
		status_pro = "404";
		return;
	}
	if (cgi_file.empty() == true)
	{
		execute_cgi(*this, config[index]);
		op_cgi.open(cgi_file.c_str());
	}
}