#include "multiplexing.hpp"
#include "request.hpp"

void Request::Delete_Function(epoll_event &event, servers &config)
{
    if (this->parse_url_prot("DELETE", config) == 1)
        return ; 

    if ((!config[index_serv].get_loc_path_location(this->Path).empty()) && (config[index_serv].get_loc_delete(this->Path) == 0) && ((is_open_diir("." + Path) == 1)))
    {
        status_pro = "405";
        flg_resp_err = 1;
        return ;
    }
    string root = get_the_p(config, Path);

	if (this->Path == "/")
	{
		if (config[index_serv].get_loc_auto_index(this->Path))
		{
			string str = config[index_serv].get_root();
			if (full_Path[0] != '/' && str[str.length() - 1] != '/')
				root_page(event, ((str + this->full_Path)));
			else
				root_page(event, ((str.erase((str.length() - 1), 1) + this->full_Path)));
		}
		else
			status_pro = "403";
	}
	else if ((check_permission_F(root) == 1) && ((check_permission_X(root) == 1)))
        delete_content(root);
    else
        status_pro = "403";
    if (status_pro != "204")
        flg_resp_err = 1;
}

void Request::delete_content(string pat)
{
    DIR* FOLDER;
    struct dirent* entre;
    struct dirent* entre1;

    FOLDER = opendir(pat.c_str());
    if (FOLDER)
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
                            delete_content(str);
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
            status_pro = "204";
        else
            status_pro = "403";
        closedir(FOLDER);
        FOLDER = NULL;
    }
    else
    {
        if (is_open_fil(pat) == 1 && check_permission_X(pat) == 1)
        {
            std::remove(pat.c_str());
            status_pro = "204";
        }
        else
            status_pro = "403";
    }
}