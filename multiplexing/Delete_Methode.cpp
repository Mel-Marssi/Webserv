#include "multiplexing.hpp"
#include "request.hpp"

void Request::Delete_Function(epoll_event &event, servers &config, int epoll_fd, map<string, string> &m)
{
	(void)event;
	(void)config;
	(void)epoll_fd;
	(void)m;
	int indx = get_right_index(config.server, atoi(_port.c_str()), _host, config.get_server_name(atoi(_port.c_str())));
	
    this->parse_url_prot("DELETE", epoll_fd, event, config);

    string root = get_the_p(config, Path, file_get);
    cout << root << "  ==  " << Path  << "  --  "<<file_get<< endl;
    // string tmp;
    // if ()

	if ((file_get == "") && (this->Path_bef[Path_bef.length() - 1] != '/'))
		response_for_delete("409", event, epoll_fd);
	else if (this->Path == "/")
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
	}// //file
	else if (is_open_diir("." + Path) == 0)
	{
		if (check_permission_F(root) == 1)
            delete_content(root, "", event, epoll_fd);
        else
        	error_page(event, epoll_fd, "404", config);
    }
    else if (is_open_diir("." + Path) == 1)
    {
		if (file_get == "")
		{
			if (check_permission_F(root) == 1)
				delete_content(root, "", event, epoll_fd);
			else
				error_page(event, epoll_fd, "404", config);
		}
		else if (file_get != "")
		{
			if (check_permission_F(root) == 1)
				delete_content(root, file_get, event, epoll_fd);
			else
				error_page(event, epoll_fd, "404", config);
		}
    }
    else //just add this -----------------------------
        error_page(event, epoll_fd, "404", config);
    //=========================================
	// else if (!config[indx].get_loc_path_location(this->Path).empty())
	// {
	// 	if (Path[0] == '/' && root[root.length() - 1] == '/')
	// 		root.erase(root.length() - 1, 1);
	// 	root += Path;
	// 	if (file_get == "")
	// 	{
	// 		if (check_permission(root) == 1)
	// 			delete_content(root, "", event, epoll_fd);
	// 		else
	// 			error_page(event, epoll_fd, "403", config);
	// 	}
	// 	else if (file_get != "")
	// 	{
	// 		if (check_permission(root) == 1)
	// 			delete_content(root, file_get, event, epoll_fd);
	// 		else
	// 			error_page(event, epoll_fd, "403", config);
	// 	}
	// }
	// else
	// 	error_page(event, epoll_fd, "403", config);
}

void Request::delete_content(string pat, string file, epoll_event& event, int epoll_fd)
{
    DIR* FOLDER;
    struct dirent* entre;
    struct dirent* entre1;

    FOLDER = opendir(pat.c_str());
    if (FOLDER)
    {
        if (file == "")
        {
            while ((entre = readdir(FOLDER)) != NULL)
            {
                string name = entre->d_name;
                if (name[0] != '.')
                {
                    string str = pat + "/" + name;
                    if (check_permission_X(str) == 1)
                    {
                        DIR* fold = opendir(str.c_str());
                        if (fold)
                        {
                            if ((entre1 = readdir(fold)) != NULL)
                                delete_content((str), "", event, epoll_fd);
                            else
                                std::remove((str).c_str());
                            closedir(fold);
                            fold = NULL;
                        }
                        else
                            std::remove((str).c_str());
                    }
                }
            }
            if (std::remove((pat).c_str()) == 0)
                response_for_delete("204", event, epoll_fd);
            else
                response_for_delete("403", event, epoll_fd);
            closedir(FOLDER);
            FOLDER = NULL;
        }
        else
        {
            string str;
            if ((pat[pat.length() - 1] == '/') && (file[0] == '/'))
                str = pat + file.erase(0,1);
            else
                str = pat + "/" + file;
            if (check_permission_F(str) == 1)
            {
                if (is_open_diir(str) == 1 && check_permission_X(str))
                    delete_content(str, "", event, epoll_fd);
                else if (is_open_fil(str) == 1 && check_permission_X(str))
                {
                    if (std::remove(str.c_str()) == 0)
                    {
                        response_for_delete("204", event, epoll_fd);
                    }
                    else
                        response_for_delete("403", event, epoll_fd);
                }
                else
                    response_for_delete("403", event, epoll_fd);
            }
            else
                response_for_delete("404", event, epoll_fd);
        }
    }
    else
    {
        if (is_open_fil(pat) == 1 && check_permission_X(pat) == 1)
        {
            std::remove(pat.c_str());
            response_for_delete("204", event, epoll_fd);
        }
        else
            response_for_delete("404", event, epoll_fd);
    }
}
