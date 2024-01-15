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
    op_cgi.seekg(0, ios::end);
    streampos fileS = op_cgi.tellg();
    op_cgi.seekg(0, ios::beg);

    char buffer[1024];
    memset(buffer, 0, 1024 );
    op_cgi.read(buffer, 1024);
    //--- size of what read is reading ---
    std::streamsize bytesRead = op_cgi.gcount();
    line.append(buffer, bytesRead);
    //------------------------------------
    head += "HTTP/1.1 200 ok\r\n";
    // con_type = get_content_type(m);
    // head += con_type;
    head += "Content-Lenght:";
    size << fileS; // check this
    head += size.str();
    head += "\r\n";
    // head += "\r\n\r\n";
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

int Request::check_permission_F(string str)
{
    if (access((str).c_str(), F_OK) != -1)
        return (1);
    return (0);
}

int Request::check_permission_X(string str)
{
    if (access((str).c_str(), X_OK) != -1)
        return (1);
    else
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
        cout << "----===\t" << str << endl;
        fil.close();
        return 1;
    }
    return 0;
}

void Request::response_for_delete(string status, epoll_event &event, int epoll_fd)
{
    string head;
    size_t len;

    head += "HTTP/1.1 ";
    head += status;
    head += get_status_code(status);
    head += "\r\n\r\n";
    len = head.length();

    send(event.data.fd, head.c_str(), len, 0);
    fin_or_still = finish;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
    close(event.data.fd);
}

string Request::get_the_p(servers &config, string Path, string file_get)
{
    (void)file_get;
	int indx = get_right_index(config.server, atoi(_port.c_str()), _host, config.get_server_name(atoi(_port.c_str())));

	string root = config[indx].get_loc_root(this->Path);
	if (root == "")
		root = config[indx].get_root();
    
    if (((Path[0] == '/') && (root[root.length() - 1] == '/')))
        root.erase(root.length() - 1, 1);

    // cout << root << "  ||==||  " << Path  << "  ||--||  "<<file_get<< endl;
    return (root);
}