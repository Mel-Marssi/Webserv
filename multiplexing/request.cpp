#include "request.hpp"
Request::Request()
{
    // cout<< "*****" <<this->_port << endl;
}
Request::Request(map<int, pair<string, string> > server_book, int fd_client)
{
    (void)server_book;
    (void)fd_client;
    flag_read_cgi = 1;

    flg_pars_url =0;
    type = "NULL";
    size_File_boundri = 0;
    // startTime = 0;
    size_chuked = 0;
    read_get = 0;
    Path = "";
    file_get = "";
    get_to_cgi = false;
    size_read = 0;
    size_read_request = 0;
    size_request = 0;
    check_left_header = 0;
    check_create_file = 0;
    fin_or_still = Still;
    check_req = 0;
    len = 0;
    fir_body = "NULL";
    dire = NULL;
    exten = "";
    check_read_get = 0;
    read_request = "";
    ck = 0;
    event_fd = 0;
    finir = 0;
    size = 0;
    total = 0;
    next_chunked = 0;
    acces_read_in_post = 0;
    err = 0;
    last = 0;
    fake_bondary = "NULL";
    wait = 0;
    status_pro = "NULL";
    size_body_get = 0;
    fd_request = 0;
    rest = 0;
    path_post = "NULL";
    cgi_post = false;
    fill_content_type();
}

Request::Request(const Request &obj)
{
    type = "NULL";
    cgi_post = false;
      flag_read_cgi = 1;
    Path = "";
    root = "";
    size_File_boundri = 0;
    // startTime = 0;
    file_get = "";
    rest = 0;
    size_chuked = 0;
    read_get = 0;
    fd_request = 0;
    size_body_get = 0;
    check_read_get = 0;
    status_pro = "NULL";
    fake_bondary = "NULL";
    wait = 0;
    last = 0;
    get_to_cgi = obj.get_to_cgi;
    event_fd = 0;
    ck = 0;
    read_request = "";
    exten = "";
    size_read = 0;
    size_read_request = 0;
    size_request = 0;
    check_left_header = 0;
    flg_pars_url =0;
    check_create_file = 0;
    // check_first_line = 0;
    kk = 0;
    len = 0;
    dire = NULL;
    err = 0;
    fin_or_still = Still;
    // check_first_time = 0;
    // counter = obj.counter;
    check_req = 0;
    fir_body = "NULL";
    finir = 0;
    size = 0;
    total = 0;
    next_chunked = 0;
    acces_read_in_post = 0;
    path_post = "NULL";
    fill_content_type();
    (void)obj;
}

int Request::parse_line(string line, int check_first)
{
    if (check_first == 0)
    {
        first_line_reque = line;
        size_t found_POST = line.find("POST");
        size_t found_GET = line.find("GET");
        size_t found_DELETE = line.find("DELETE");
        if (found_POST != string::npos)
        {
            header_request.insert(make_pair(line.substr(0, found_POST + 4), (line.substr(found_POST + 5, line.length()).c_str())));
            this->methode = POST;
        }
        else if (found_GET != string::npos)
        {
            header_request.insert(make_pair(line.substr(0, found_GET + 3), (line.substr(found_GET + 4, line.length()).c_str())));
            this->methode = GET;
        }
        else if (found_DELETE != string::npos)
        {
            header_request.insert(make_pair(line.substr(0, found_DELETE + 6), (line.substr(found_DELETE + 7, line.length()).c_str())));
            this->methode = DELETE;
        }
        else
        {
            if (line.find("HEAD") != string::npos)
                methode = "HEAD";
            else
                this->methode = NONE;
        }
    }
    else
    {
        size_t found_POINT = line.find(":");
        if (found_POINT != string::npos)
        {
            if (line[found_POINT + 1] == ' ')
                header_request.insert(make_pair(line.substr(0, found_POINT), (line.substr(found_POINT + 2, line.length() - 1).c_str())));
            else
                header_request.insert(make_pair(line.substr(0, found_POINT), (line.substr(found_POINT + 1, line.length() - 1).c_str())));
        }
    }
    return 0;
}

int Request::parce_request(string read_request, epoll_event &event, int epoll_fd, servers &config)
{
    (void)event;
    (void)epoll_fd;
    (void)config;

    size_t i = 0;
    if (check_left_header == 1)
    {
        return 1;
    }
    size_t check_body = read_request.find("\r\n\r\n");
    if (check_body == 0)
    {
        status_pro = "400";
        check_left_header = 1;
        methode = "POST";
        return 0;
    }
    string tmp;
    if (check_body != string::npos)
    {
        if ((read_request[check_body] != '\r' || read_request[check_body + 1] != '\n' || read_request[check_body + 2] != '\r' || read_request[check_body + 3] != '\n'))
        {

            status_pro = "400";
        }
        check_left_header = 1;
    }

    check_body++;
    string line;
    int check_first_line = 0;
    while (read_request[i] && i <= check_body) // && i <= check_body)//&& i <= check_body
    {
        if (read_request[i] != '\n')
            line += read_request[i];
        else
        {
            parse_line(line, check_first_line);
            line = "";
            check_first_line = 1;
        }
        i++;
    }
    string l = header_request["Content-Length"];
    size_request = atol(l.c_str());
    if (check_body + 3 < read_request.length())
    {
        this->fir_body = read_request.substr(check_body + 3, read_request.length());
        size_body_get += size_read_request - check_body + 3;
    }
    read_request.clear();
    return 0;
}

void Request::fill_content_type()
{
    cont_type["text/plain\r"] = ".txt";
    cont_type["image/gif\r"] = ".gif";
    cont_type["text/html\r"] = ".html";
    cont_type["image/jpeg\r"] = ".jpeg";
    cont_type["image/png\r"] = ".png";
    cont_type["application/json\r"] = ".json";
    cont_type["text/javascript\r"] = ".js";
    cont_type["audio/mpeg\r"] = ".mp3";
    cont_type["video/mp4\r"] = ".mp4";
    cont_type["video/mpeg\r"] = ".mpeg";
    cont_type["application/pdf\r"] = ".pdf";
    cont_type["video/webm\r"] = ".webm";
    cont_type["application/zip\r"] = ".zip";
    cont_type["application/x-php\r"] = ".php";
    cont_type["application/x-httpd-php\r"] = ".php";
    cont_type["text/x-python\r"] = ".py";
    cont_type["application/x-shellscript\r"] = ".sh";
}

void Request::create_file(ofstream &outputFile, map<string, string> &map, servers &config, int index)
{

    string type_file = map["Content-Type"];
    std::map<string, string>::iterator it = header_request.find(type_file);
    srand(time(NULL));
    ostringstream str;
    str << rand();
    if (it != cont_type.end() && !cont_type[type_file].empty())
    {
        string randomName = config[index].get_loc_up_folder(Path) + "/" + str.str() + cont_type[type_file];
        file_name_post = randomName;
        path_post = randomName;
        outputFile.open(randomName.c_str());
    }
    else
    {
         status_pro = "415";
        string randomName = config[index].get_loc_up_folder(Path) + "/" + str.str()  + ".txt";
        path_post = randomName;
        file_name_post = randomName;
        outputFile.open(randomName.c_str());
    }
}

void Request::binary(servers &config, int index)
{
    if (check_create_file == 0)
    {
        map<string, string>::iterator oo = header_request.find("Content-Type");
        if (oo != header_request.end() && !header_request["Content-Type"].empty())
            create_file(outputFile, header_request, config, index);
        check_create_file = 1;
        if (fir_body != "NULL")
        {
            // cout << fir_body << endl;
            if (fir_body.length() > (size_t)atol(header_request["Content-Length"].c_str()))
            {
                outputFile << fir_body.substr(0, atol(header_request["Content-Length"].c_str()));
            }
            else
                outputFile << fir_body;
            fir_body = "NULLLL";
            fir_body.clear();
        }
        else
            outputFile << read_request;
    }
    else
    {
        outputFile << read_request;
    }
    read_request.clear();
}

int Request::Handle_error(int fd, servers &config, epoll_event &event)
{
    (void)fd;
    (void)event;
    map<string, string>::iterator it = header_request.find("Transfer-Encoding");
    map<string, string>::iterator it0;
    it0 = header_request.find("Content-Length"); // Content-Length
    if (this->parse_url_prot("POST", config) == 1)
        return 1;
    // int index = get_right_index(config.server, atoi(_port.c_str()), _host, _host);

    if ((config[index_serv].get_loc_path_location(this->Path).empty()))
    {
        status_pro = "404";
        return 1;
    }
    //========================================
    if ((it == header_request.end()) && (it0->second.empty()))
    {
        status_pro = "400";
        return 1;
    }

    if ((config[index_serv].get_loc_post(this->Path) == 0) && (is_open_diir(Path) == 0)) // && (event.events & EPOLLOUT) && (is_open_diir(Path) == 0)
    {
        status_pro = "405";
        return 1;
    }
    if (config[index_serv].get_loc_upload(this->Path) == 0) //&& (events & EPOLLOUT )
    {
        status_pro = "404";
        return 1;
    }
    return 0;
}

void Request::chunked(servers &config, int index)
{

    if (check_create_file == 0)
    {
        map<string, string>::iterator itC = header_request.find("Content-Type");
        if (itC != header_request.end()&& !header_request["Content-Type"].empty())
            create_file(outputFile, header_request, config, index);
        check_create_file = 1;

        if (fir_body != "NULL")
        {
            if (fir_body == "0\r\n\r\n")
            {
                finir = 1;
            }
            int position_int = fir_body.find("\r\n");
            string num = fir_body.substr(0, position_int);
            char *endptr;

            size_chunked = strtol(num.c_str(), &endptr, 16);

            fir_body = fir_body.substr(position_int + 2, fir_body.length());
            size_t check_last = fir_body.find("\r\n0\r\n\r\n");
            if (check_last != string::npos)
            {
                finir = 1;
                fir_body = fir_body.substr(0, check_last);
            }
            outputFile << fir_body;
            size_chuked += fir_body.length();
            size_chunked -= fir_body.length();
            fir_body = "NULL";
        }

        read_request.clear();
    }

}

string Request::generat_name(string name, servers &config, int index, string content)
{
    int i = 1;
    string generat = config[index].get_loc_up_folder(Path) + "/" + name + cont_type[content];
    if (access(generat.c_str(), F_OK) == -1)
        return generat;
    while (1)
    {
        stringstream ss;
        ss << i;
        string str = ss.str();
        generat = config[index].get_loc_up_folder(Path) + "/" + name + "(" + str + ")" + cont_type[content];
        if (access(generat.c_str(), F_OK) == -1)
            return generat;
        i++;
    }
}

void Request::create_file_bondar(ofstream &outputFile, map<string, string> &map, servers &config, int index, string name, string content)
{
    (void)map;
    content = content + "\r";
    string type_file = content;

    std::map<string, string>::iterator it = cont_type.find(type_file);
    srand(time(NULL));
    ostringstream str;
    str << rand();

    if (it != cont_type.end())
    {
        // name  = generat_name(name, config, index, content);
        string randomName = generat_name(name, config, index, content); // config[index].get_loc_up_folder(Path) + "/" + name   + cont_type[content];
                                                                             //  cout << randomName << endl;
        path_post = randomName;
        files.push_back(randomName);

        outputFile.open(randomName.c_str());
    }
    else
    {
        // string randomName = str.str() + ".x";
        string randomName = generat_name(name, config, index, "text/plain\r"); // config[index].get_loc_up_folder(Path) + "/" + name   + ".txt";
                                                                                    //  cout << randomName<<"\n";
        path_post = randomName;

        outputFile.open(randomName.c_str());
    }

    //  exit(1);
}

void Request::boundar(servers &config, int index)
{

    if (check_create_file == 0)
    {
        string content = header_request["Content-Type"];

        boudri = content.substr(content.find("=") + 1, content.length() - (content.find("=") + 1) - 1);
        last_boundri = boudri;
        last_boundri += "--";
        if (fir_body != "NULL")
        {
            size_t position_int = fir_body.find("\r\n\r\n");

            string boundary = fir_body.substr(0, position_int);
            // cout << boundary << endl;
            string filename_content = boundary.substr(boundary.find("name=\""));
            string l = filename_content.substr(6, filename_content.length());
            string filename = filename_content.substr(6, l.find("\""));
            string Content = "NULL";
            if (filename_content.find("Content-Type") != string::npos)
                Content = filename_content.substr(filename_content.find("\r\n") + 16);
            // size_File_boundri = 0;

            outputFile.close();
            create_file_bondar(outputFile, header_request, config, index, filename, Content);
            check_create_file = 1;
            fir_body = fir_body.substr(position_int + 4, fir_body.length());
            // exit(1);
            size_t found = fir_body.find(boudri);
            if (found != string::npos)
            {
                string push = fir_body.substr(0, found - 2);
                // cout << push << endl;
                fir_body = fir_body.substr(found);
                if (fir_body.substr(0, fir_body.length() - 2) == last_boundri)
                {
                    finir = 1;
                }
                else
                {
                    fake_bondary = fir_body;
                    size_read_request = 0;
                }
                outputFile << push;
                size_File_boundri += push.length();
            }
            else
                fake_bondary = fir_body;
            fir_body = "NULLLL";
        }
        else
        {
            outputFile << read_request;
            size_File_boundri += read_request.length();

            //    read_request.erase();
        }
    }
    else
    {
        outputFile << read_request;
    }
}

void Request::boundaries(servers &config, int index, int fd, epoll_event &event)
{
    char buff[1024];
    memset(buff, 0, 1024);
    if ((config[index].get_loc_max_client_size(this->Path) < (size_t)size_File_boundri))
    {
        outputFile.close();
        size_read_request = 0;
        finir = 0;
        vector<string>::iterator itt = files.begin();
        for (; itt != files.end(); itt++)
            remove((*itt).c_str());
        status_pro = "413";
        fake_bondary = "NULL";
        return;
    }
    if (acces_read_in_post == 1)
    {
        int a = 0;
        if ((event.events & EPOLLIN))
        {
            a = read(fd, buff, 1024);
            if (a < 0)
            {
                status_pro = "500";
                vector<string>::iterator itt = files.begin();
                for (; itt != files.end(); itt++)
                    remove((*itt).c_str());
                fake_bondary = "NULL";
                return ;
            }
        }
        read_request.clear();
        if (fake_bondary != "NULL")
        {
            read_request = fake_bondary;
            fake_bondary = "NULL";
        }
        if (a > 0)
            read_request.append(buff, a);
        size_t found_bondary = read_request.find(boudri);
        if (found_bondary == string::npos) // mochkil kayn akhir boundri kaydkhol hna
        {
            fake_bondary = read_request.substr(read_request.length() - boudri.length()); // , read_request.length()
            read_request = read_request.substr(0, read_request.length() - boudri.length());
            outputFile << read_request;
            size_File_boundri += read_request.length();
        }
        else
        {
            fake_bondary = "NULL";
            size_t _last_found_bondary = read_request.find(last_boundri);
            if (_last_found_bondary == found_bondary)
            {
                string push = read_request.substr(0, _last_found_bondary - 2);
                outputFile << push;
                size_File_boundri += push.length();

                finir = 1;
            }
            else
            {
                size_t position_int = read_request.find("\r\n\r\n");
                if (position_int == string::npos)
                {
                    fake_bondary = read_request;
                    return;
                }
                string boundary = read_request.substr(0, position_int);
                if (boundary.find("name=\"") == string::npos)
                {
                    fake_bondary = read_request.substr(position_int + 4);
                    return;
                }
                string filename_content = boundary.substr(boundary.find("name=\""));
                string l = filename_content.substr(6, filename_content.length());
                string filename = filename_content.substr(6, l.find("\""));
                string Content = "NULL";
                if (filename_content.find("Content-Type") != string::npos)
                    Content = filename_content.substr(filename_content.find("\r\n") + 16);
                if (found_bondary)
                {
                    string push = read_request.substr(0, found_bondary - 2);
                    outputFile << push;
                    size_File_boundri += push.length();
                }
                // size_File_boundri = 0;
                outputFile.close();
                create_file_bondar(outputFile, header_request, config, index, filename, Content);

                read_request = read_request.substr(position_int + 4);
                if (read_request.substr(0, read_request.length() - 2) == last_boundri)
                    finir = 1;
                else
                {
                    fake_bondary = read_request;
                    // size_read_request = -5;
                }
            }
            // cout << fake_bondary<<"\n";
            read_request.clear();
        }
        if ((config[index].get_loc_max_client_size(this->Path) < (size_t)size_File_boundri))
        {
            outputFile.close();
            size_read_request = 0;
            finir = 0;
            vector<string>::iterator itt = files.begin();
            for (; itt != files.end(); itt++)
                remove((*itt).c_str());
            status_pro = "413";
            fake_bondary = "NULL";
            return;
        }
        return;
        // exit(1);
    }
    if (check_left_header == 1)
        acces_read_in_post = 1;
    boundar(config, index);
    if ((config[index].get_loc_max_client_size(this->Path) < (size_t)size_File_boundri) )
    {
        outputFile.close();
        size_read_request = 0;
        finir = 0;
        vector<string>::iterator itt = files.begin();
        for (; itt != files.end(); itt++)
            remove((*itt).c_str());
        status_pro = "413";
        fake_bondary = "NULL";
        return;
    }
}

void Request::post(int fd, servers &config, epoll_event &event)
{
    map<string, string>::iterator it = header_request.find("Transfer-Encoding");
    map<string, string>::iterator itC = header_request.find("Content-Type");
// cout << atol(header_request["Content-Length"].c_str()) << endl;
  
    if ((fir_body != "NULL" || atol(header_request["Content-Length"].c_str()) > 0 || (event.events & EPOLLIN)) && status_pro != "NULL")
    {
        size_read_request = 0;
        err = 0;
        int s = 0;
        char buff[2048];

        if (check_read_get == 1)
        {
            s = read(event.data.fd, buff, 2048);
            if (s < 0)
            {
                status_pro = "500";
                return ;
            }
            size_body_get += s;
            read_request.clear();
            read_request.append(buff, s);
        }
        if (header_request["Transfer-Encoding"] == "chunked\r" && read_request.find("\r\n0\r\n\r\n") != string::npos)
        {
            finir = 1;
        }
        else if (header_request["Transfer-Encoding"] != "chunked\r"  && header_request["Content-Type"].find("multipart/form-data") == string::npos && size_body_get >= (size_t)atol(header_request["Content-Length"].c_str()))
        {
            finir = 1;
        }
        else if (header_request["Content-Type"].find("multipart/form-data") != string::npos && read_request.find(last_boundri) != string::npos) // chof test lia kayn f postman boundries
        {
            finir = 1;
        }

        return;
    }
    if (it != header_request.end() && header_request["Transfer-Encoding"] != "chunked\r")
    {
        status_pro = "501";
        return;
    }
    if (check_read_get == 0)
    {
        this->parse_url_prot("POST", config);
        string content = header_request["Content-Type"];
        size_t si = content.find("multipart/form-data");
        if (it != header_request.end() && header_request["Transfer-Encoding"] == "chunked\r")
        {
            type = "chunked";
        }
        else if (si != string::npos)
        {
            type = "boundri";
            // return;
        }
        else
         type = "binary";
    }

    if (type == "chunked")
    {
        if ((config[index_serv].get_loc_max_client_size(this->Path) < (size_t)size_chuked) || status_pro == "504")
        {
            outputFile.close();
            // size_read_request = 0;
            finir = 0;
            remove(file_name_post.c_str());
            if (status_pro != "504")
                status_pro = "413";
            return;
        }
        char buff[2048];
        memset(buff, 0, 2048);
        if (acces_read_in_post == 1)
        {
            if (next_chunked == 1)
            {
                int a = 0;
                if (rest == 0)
                    size = 0;
                a = read(fd, buff, 2048 - size);
                if (a < 0)
                {
                    outputFile.close();
                    size_read_request = 0;
                    finir = 1;
                    remove(file_name_post.c_str());
                    status_pro = "500";
                    return;
                }
                size_chuked += a;
                if (a < 2048 - size)
                {
                    rest = 1;
                    size += a;
                    read_request.append(buff, a);
                    size_t end_chunked = read_request.find("\r\n0\r\n\r\n");
                    if (end_chunked != string::npos)
                    {
                        read_request = read_request.substr(0, end_chunked);
                        // outputFile << read_request;
                        outputFile.write(read_request.c_str(), read_request.length());
                        read_request.clear();
                        finir = 1;
                    }
                    return;
                }
                if (size_chunked == total)
                {
                    size += a;
                    read_request.append(buff, a);
                    read_request = read_request.substr(2, size);
                    size_t position_int = read_request.find("\r\n");
                    string num = read_request.substr(0, position_int);
                    read_request = read_request.substr(position_int + 2, size);
                    // outputFile << read_request;
                    outputFile.write(read_request.c_str(), read_request.length());

                    char *endptr;
                    size_chunked = strtol(num.c_str(), &endptr, 16);

                    if (size_chunked == 0)
                    {
                        finir = 1;
                    }
                    total = read_request.length();
                    rest = 0;
                    read_request.clear();
                    next_chunked = 0;
                    size = 0;
                }
                return;
            }
            else
            {
                if (size_chunked - total >= 2048)
                {
                    size = read(fd, buff, 2048);
                    if (size < 0)
                    {
                        outputFile.close();
                        size_read_request = 0;
                        finir = 1;
                        remove(file_name_post.c_str());
                        status_pro = "500";
                        return;
                    }
                    size_chuked += size;
                    total += size;
                    read_request.append(buff, size);
                    // outputFile << read_request;
                    outputFile.write(buff, size);

                    read_request.clear();
                    size = 0;
                    return;
                    // size_chunked -= size;
                }
                else if (size_chunked - total > 0)
                {
                    size = read(fd, buff, size_chunked - total);
                    if (size < 0)
                    {
                        outputFile.close();
                        size_read_request = 0;
                        finir = 1;
                        remove(file_name_post.c_str());
                        status_pro = "500";
                        return;
                    }
                    total += size;
                    size_chuked += size;
                    if (size == (size_chunked - total))
                    {
                        read_request.append(buff, size);
                        outputFile.write(buff, size);
                        // outputFile << read_request;
                        if (size == (size_chunked - total))
                        {
                            size = 0;
                            next_chunked = 1;
                        }
                        // total = 0;
                        size = 0;
                        read_request.clear();

                        return;
                    }
                    else
                    {
                        read_request.append(buff, size);
                        // outputFile << read_request;
                        outputFile.write(buff, size);
                        read_request.clear();
                        size = 0;

                        return;
                    }
                }
                else
                {
                    size = 0;
                    next_chunked = 1;
                    return;
                }
                size = 0;
                if (total >= size_chunked)
                {
                    next_chunked = 1;
                    return;
                }
                return;
            }
        }
        chunked(config, index_serv);
        if (check_left_header == 1)
            acces_read_in_post = 1;
         if ((config[index_serv].get_loc_max_client_size(this->Path) < (size_t)size_chuked) || status_pro == "415")
        {
            outputFile.close();
            // size_read_request = 0;
            finir = 0;
            remove(file_name_post.c_str());
             if (status_pro != "415")
                status_pro = "413";
            return;
        }
    }

    else if (fir_body != "NULL")
    {
        if (type == "boundri")
        {
            boundaries(config, index_serv, fd, event);
        }
        else if (type == "binary")
        {
            if ((config[index_serv].get_loc_max_client_size(this->Path) < (size_t)size_request)  || status_pro == "415")
            {
                // cout << "kddk\n";

                outputFile.close();
                // size_read_request = 0;
                finir = 0;
                remove(file_name_post.c_str());
                if (status_pro != "415")
                    status_pro = "413";
                return;
            }
            if (!(event.events & EPOLLIN))
            {
                size_read_request = size_request;
                return;
            }
            if (acces_read_in_post == 1 && event.events & EPOLLIN)
            {

                char buff[2048];

                size = 0;
                size = read(fd, buff, 2048);
                if (size < 0)
                {
                    outputFile.close();
                    size_read_request = 0;
                    finir = 1;
                    remove(file_name_post.c_str());
                    status_pro = "500";
                    return;
                }
                size_read_request += size;
                read_request.append(buff, size);
                outputFile << read_request;
                read_request.clear();
                return;
            }
            binary(config, index_serv);
              if (status_pro == "415")
            {
                outputFile.close();
                finir = 0;
                remove(file_name_post.c_str());
                return;
            }
            if (check_left_header == 1)
                acces_read_in_post = 1;
        }
    }

    else if (itC != header_request.end())
        create_file(outputFile, header_request, config, index_serv);
}