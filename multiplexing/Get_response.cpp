#include "multiplexing.hpp"
#include "request.hpp"

std::string Request::read_buff(map<string, string> &m)
{
    string head;
    stringstream size;
    // //to get the size =======
    op.seekg(0, ios::end);
    streampos fileS = op.tellg();
    op.seekg(0, ios::beg);

    char buffer[1024];
    memset(buffer, 0, 1024 );
    op.read(buffer, 1024);
    //--- size of what read is reading ---
    std::streamsize bytesRead = op.gcount();
    line.append(buffer, bytesRead);
    //------------------------------------
    head += "HTTP/1.1 200 ok\r\nContent-Type: ";
    con_type = get_content_type(m);
    head += con_type;
    head += "\r\nContent-Lenght:";
    size << fileS;
    head += size.str();
    head += "\r\n\r\n";
    head += line;
    line = "";
    return (head);
}

std::string Request::read_buff_cgi(map<string, string> &m)
{
    (void)m;
    string head;
    stringstream size;
    // //to get the size =======
    op.seekg(0, ios::end);
    streampos fileS = op.tellg();
    op.seekg(0, ios::beg);

    char buffer[1024];
    memset(buffer, 0, 1024 );
    op.read(buffer, 1024);
    //--- size of what read is reading ---
    std::streamsize bytesRead = op.gcount();
    line.append(buffer, bytesRead);
    //------------------------------------
    head += "HTTP/1.1 200 ok\r\n";
    // con_type = get_content_type(m);
    // head += con_type;
    head += "Content-Lenght:";
    size << fileS;
    head += size.str();
    head += "\r\n\r\n";
    head += line;
    line = "";
    return (head);
}

void Request::read_for_send(map<string, string> &m, int flg)
{
    string head;
    if (flg == 0)
    {
        check_req = 1;
        head = read_buff(m);
        len = head.length();
    }
    else
    {
        check_req = 1;
        
        head = read_buff_cgi(m);
        len = head.length();
    }

    send(event_fd, head.c_str(), len, 0);
    line = "";
}

void Request::end_of_file(epoll_event &event, int epoll_fd)
{
    fin_or_still = finish;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
    close(event_fd);
    op.close();
    close_dir();
}

void Request::close_dir()
{
    if (dire)
    {
        closedir(dire);
        dire = NULL;
    }
}

int Request::check_permission(string str)
{
    if (access((str).c_str(), F_OK) != -1)
    {
        if ((access((str).c_str(), R_OK) && access((str).c_str(), W_OK) && access((str).c_str(), X_OK)) == 0)
            return (1);
        else
            return (0);
    }
    return (0);
}

int Request::is_open_diir(string str)
{
    DIR* d;
    d = opendir(str.c_str());
    if (d)
    {
        closedir(d);
        d = NULL;
        return 1;
    }
    return 0;
}

int Request::is_open_fil(string str)
{
    ifstream fil(str.c_str());
    if (fil.is_open())
    {
        fil.close();
        return 1;
    }
    return 0;
}

void Request::delete_content(string pat, string file)
{
    DIR* FOLDER;
    struct dirent* entre;
    struct dirent* entre1;
    // if (file_get == "")
    // {
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
                        if (check_permission(str) == 1)
                        {
                            DIR* fold = opendir(str.c_str());
                            if (fold)
                            {
                                if ((entre1 = readdir(fold)) != NULL)
                                    delete_content((str), "");
                                else
                                    std::remove((str).c_str());
                                closedir(fold);
                                fold = NULL;
                            }
                            else
                                std::remove((str).c_str());
                        }
                        else
                            cout << "Error Page__\n";
                    }
                }
                close_dir();
                std::remove((pat).c_str());
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
                // cout << str << " -----\n";
                if (is_open_diir(str) == 1)
                    delete_content(str, "");
                else if (is_open_fil(str) == 1)
                    std::remove(str.c_str());
                else
                    cout << "Error Page==\n";
            }
        }
        else
        {
            if (is_open_fil(pat) == 1)
                std::remove(pat.c_str());
            else
                cout << "Error Page\n";
        }
        (void)file;
}