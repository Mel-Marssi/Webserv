#include "multiplexing.hpp"
#include "request.hpp"

void Request::Delete_Function(epoll_event &event, servers &config, int epoll_fd, map<string, string> &m)
{
	(void)event;
	(void)config;
	(void)epoll_fd;
	(void)m;
	// this->parse_url_prot("DELETE");

	// // cout << Path << endl;
	// // cout << full_Path << endl;
	// // cout << file_get << endl;
	// // int indx = get_right_index(config.server, atoi(_port.c_str()), _host, config.get_server_name(atoi(_port.c_str())));

	// if (!config[indx].get_loc_path_location(this->Path).empty())
}