#include "multiplexing.hpp"
#include "request.hpp"

void Request::Delete_Function(epoll_event &event, servers &config, int epoll_fd, map<string, string> &m)
{
	(void)event;
	(void)config;
	(void)epoll_fd;
	(void)m;
	int indx = get_right_index(config.server, atoi(_port.c_str()), _host, config.get_server_name(atoi(_port.c_str())));
	this->parse_url_prot("DELETE");

	string root = config[indx].get_loc_root(this->Path);
	if (root == "")
		root = config[indx].get_root();

	// cout << Path << endl;
	// cout << full_Path << endl;
	// cout << file_get << endl;
	if (this->Path == "/")
	{
		if (config[indx].get_loc_auto_index(this->Path))
		{
			string str = config[indx].get_root();
			if (full_Path[0] != '/' && str[str.length() - 1] != '/')
				root_page(event, epoll_fd, ((str + this->full_Path)));
			else
				root_page(event, epoll_fd, ((str.erase((str.length() - 1), 1) + this->full_Path)));
		}
		else//forbiden 
			error_page(event, epoll_fd, "403", config);
	}
	//Folder
	else if (!config[indx].get_loc_path_location(this->Path).empty())
	{
		if (Path[0] == '/' && root[root.length() - 1] == '/')
			root.erase(root.length() - 1, 1);
		root += Path;
		// not have a file
		if (file_get == "")
		{
			// cout << root <<"---\n";
			if (check_permission(root) == 1)
				delete_content(root, "");
			else
				error_page(event, epoll_fd, "403", config);
		}
		//hase file at the end
		else if (file_get != "")
		{
			if (check_permission(root) == 1)
				delete_content(root, file_get);
			else
				error_page(event, epoll_fd, "403", config);
		}
	}
	// //file
	// else if (file_get == "")
	// {}
}