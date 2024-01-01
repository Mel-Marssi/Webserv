#include "request.hpp"
Request::Request(){
    // cout<< "*****" <<this->_port << endl;
}
Request::Request(map<int, pair<string, string> > server_book, int fd_client)
{
(void)server_book;
(void)fd_client;

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
    read_request = "";
    ck = 0;
    event_fd = 0;
        finir = 0;
      size = 0;
      total = 0;
      next_chunked = 0;
      acces_read_in_post = 0;
      err = 0;
      fill_content_type();
}

Request::Request(const Request& obj)
{
    event_fd = 0;
    ck = 0;
    read_request = "";
    exten = "";
    size_read = 0;
    size_read_request = 0;
    size_request = 0;
    check_left_header = 0;
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
      fill_content_type();
      (void) obj;
}
 


int Request::parse_line(std::string line, int check_first)
{
    // cout << line << endl;

    if (check_first == 0)
    {
        size_t found_POST = line.find("POST");
        size_t found_GET = line.find("GET");
        size_t found_DELETE = line.find("DELETE");
        if (found_POST != std::string::npos)
        {
            header_request.insert(std::make_pair(line.substr(0,found_POST+4),   (line.substr(found_POST + 5,line.length()).c_str())));
            this->methode = POST;
            // cout << "00000000000000000000000000\n";
        }
        else if (found_GET != std::string::npos)
        {
            header_request.insert(std::make_pair(line.substr(0,found_GET+3),   (line.substr(found_GET + 4,line.length()).c_str())));
            this->methode = GET;
            // cout << "111111111111111111111111\n";
        }
        else if (found_DELETE != std::string::npos)
        {
            header_request.insert(std::make_pair(line.substr(0,found_DELETE+6),   (line.substr(found_DELETE + 7,line.length()).c_str())));
            this->methode = DELETE;
        }
        else
        {
            this->methode = NONE;
        }
    }
    else
    {
        size_t found_POINT = line.find(":");
        // cout << found_POINT << " " << line.length() << " ---\n";
        header_request.insert(std::make_pair(line.substr(0,found_POINT),   (line.substr(found_POINT + 2,line.length() - 1).c_str())));
    }
    return 0;
}

int Request::parce_request(std::string read_request)
{
 
    size_t i = 0;
    if (check_left_header == 1)
    {
        return 1;
    }
    size_t   check_body = read_request.find("\r\n\r\n");
    if (check_body != std::string::npos)
    {
        check_left_header = 1;
    }
        
    check_body++;
    std::string line;
    int check_first_line = 0;
    while (read_request[i] && i <= check_body)
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
    std::string l = header_request["Content-Length"];
		size_request = atol(l.c_str());
        if (check_body != std::string::npos)
        {
            this->fir_body = read_request.substr(check_body + 3, read_request.length());
            // size = size - (check_body + 3);
            // cout << fir_body << endl;
        }
    
    return 0;
 
}
 

void Request::fill_content_type()
{
    cont_type["text/plain\r"] = ".txt";
    cont_type["image/gif\r"] = ".gif";
    cont_type["text/html\r"] = ".html";
    cont_type["image/jpeg\r"] = ".jpeg";
    cont_type["application/json\r"] = ".json";
    cont_type["text/javascript\r"] = ".js";
    cont_type["audio/mpeg\r"] = ".mp3";
    cont_type["video/mp4\r"] = ".mp4";
    cont_type["video/mpeg\r"] = ".mpeg";
    cont_type["application/pdf\r"] = ".pdf";
    cont_type["video/webm\r"] = ".webm";
    cont_type["application/zip\r"] = ".zip";
}


void Request::create_file(std::ofstream& outputFile,   std::map<std::string, std::string>& map, servers &config, int index)
{
 
   std::string type_file = map["Content-Type"];
   
     std::map<std::string, std::string>::iterator it = header_request.find(type_file);
    srand (time(NULL));
    std::ostringstream str;
    str << rand();

    if (it != cont_type.end())
    {
        std::string randomName =     config[index].get_loc_up_folder(Path) + "/" + str.str() + cont_type[type_file];
        outputFile.open(randomName.c_str());

    }  
    else  
    {
        std::string randomName = str.str() + ".x";
        outputFile.open(randomName.c_str());
    }
}


void Request::binary(servers &config, int index)
{
    if (check_create_file == 0)
	{
		create_file(outputFile, header_request, config,   index);
		check_create_file = 1;
		if (fir_body != "NULL")
		{
			outputFile << fir_body;
			fir_body = "NULL";
		}
		else
		{
			cout <<"****************************************\n";

			outputFile << read_request;
		}
	}
	else
	{
		outputFile << read_request;			
	}
}


void Request::chunked(servers &config, int index)
{
           

 
    if (check_create_file == 0)
	{
		create_file(outputFile, header_request, config,  index);
		check_create_file = 1;

        if (fir_body != "NULL")
		{
            int position_int = fir_body.find("\r\n"); 
            std::string num = fir_body.substr(0, position_int);
              char *endptr;
             
            size_chunked =  strtol(num.c_str(), &endptr, 16);
        
            fir_body = fir_body.substr(position_int + 2, fir_body.length());
			  outputFile << fir_body;
                // total += (size - position_int + 2);
             size_chunked -= fir_body.length();
			fir_body = "NULL";
		}
		else
		{
			  outputFile << read_request;
        //    read_request.erase();
		}
	}
	else
	{
		outputFile << read_request;			
	}
}

void Request::post(int fd, servers &config, epoll_event &event)
{
     std::map<std::string, std::string>::iterator it = header_request.find("Transfer-Encoding");
   
    //  (void)fd;
    this->parse_url_prot("POST");
    int index = get_right_index(config.server, atoi(_port.c_str()), _host, config.get_server_name(atoi(_port.c_str())));
    // exit(4);
    if ((config[index].get_loc_post(this->Path) == 0) && (Path.find(".") == string::npos))
    {
        err = 1;
        error_page(event, fd, "405", config);
        return;
    }
    cout << config[index].get_loc_upload(this->Path) << endl;
    if ( config[index].get_loc_upload(this->Path) == 0)
    {
        err = 1;
        error_page(event, fd, "403", config);
        return;
    }

     if (it != header_request.end() && header_request["Transfer-Encoding"] == "chunked\r")
     {
         char buff[1024];
        memset(buff, 0, 1024);
        if (acces_read_in_post == 1)
        {
            if (next_chunked == 1)
            {
                int a = 0;
                a = read(fd, buff, 1024 - size);
                if (a < 1024 - size)
                {
                    size += a;
                    // total+= size;
                    read_request.append(buff,a);

                    if ( size_chunked == total)
                   {
                            string read_request2;
                        if (read_request[0] == '\r')
                        {
                            read_request2 = read_request.substr(2,size);
                        }
                        else
                        {
                            read_request2 = read_request.substr(0,size);
    
                        }

                        size_t position_int = read_request2.find("\r\n"); 
                        if (position_int == string::npos)
                            return;
                        std::string num = read_request2.substr(0, position_int);
                        char *endptr;
                        size_chunked =  strtol(num.c_str(), &endptr, 16);
                        // cout << size_chunked << " -- " << total << endl;

                        if (size_chunked == 0)
                        {
                        
                            cout << size << "--"<< endl;
                            finir = 1;
                            return;
                        }
                   }
                }
               else
               {
                    size = a;
                    read_request.append(buff,a);
                    string read_request2;
                    if (read_request[0] == '\r')
                    {
                        read_request2 = read_request.substr(2,size);
                    }
                    else
                    {
                        read_request2 = read_request.substr(0,size);
 
                    }

                    int position_int = read_request2.find("\r\n"); 
                    std::string num = read_request2.substr(0, position_int);
                    char *endptr;
                    size_chunked =  strtol(num.c_str(), &endptr, 16);
                    // cout << size_chunked << " -- " << total << endl;

                    if (size_chunked == 0)
                    {
                       
                        // cout << size << "--"<< endl;
                        finir = 1;
                        return;
                    }
                    read_request2 = read_request2.substr(position_int + 2, read_request2.length());
                    outputFile << read_request2;
                    total = read_request2.length();
                    next_chunked = 0;
                    // total = 0;
                    read_request.clear();
               }
 
            }
            
     
        else
        {
            

         
            if (size_chunked - total >= 1024) 
            {
                size = read(fd, buff, 1024);
                total+=size;
                read_request.append(buff,size);
                outputFile << read_request;	
                read_request.clear();
                size = 0;
                return;
                // size_chunked -= size;
            }
            else if (size_chunked - total > 0)
            {
                // cout << size_chunked - total << " //// \n";
                size = read(fd, buff, size_chunked - total);
                total+=size;
                if (size ==  (size_chunked - total))
                {
                    read_request.append(buff,size);
                    outputFile << read_request;
                    size = 0;
                    read_request.clear();
                    // total = 0;
                    next_chunked = 1;

                    return;
                }
                else
                {
                    read_request.append(buff,size);
                    outputFile << read_request;	
                    read_request.clear();
                    size = 0;

                    return;
                }
                
            }
            else
            {
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
        chunked(config, index);
        if (check_left_header == 1)
            acces_read_in_post = 1;
}
 
     

     else
     {
        cout << "ana binary\n" << header_request["Content-Type"] << endl;

        if (acces_read_in_post == 1)
        {

            char buff[1024];
        
            size = 0;
            size = read(fd, buff, 1024);
            size_read_request += size;
            read_request.append(buff,size);
            outputFile << read_request;
            return;
                        // parce_request(read_request);
        }
        binary(config, index);
        if (check_left_header == 1)
            acces_read_in_post = 1;
     }
     
}